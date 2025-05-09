
#include "SkeletalMeshComponent.h"

#include "ReferenceSkeleton.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Asset/SkeletalMeshAsset.h"

USkeletalMeshComponent::USkeletalMeshComponent()
{
    AnimSequence = new UAnimSequence();
}

USkeletalMeshComponent::~USkeletalMeshComponent()
{
    if (AnimSequence)
    {
        delete AnimSequence;
        AnimSequence = nullptr;
    }
}



void USkeletalMeshComponent::TickComponent(float DeltaTime)
{
    USkinnedMeshComponent::TickComponent(DeltaTime);

    if (bPlayAnimation)
    {
        ElapsedTime += DeltaTime;
    }
    
    BoneTransforms = BoneBindPoseTransforms;
    
    if (bPlayAnimation && AnimSequence && SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
    {
        const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();

        const int32 AnimationFrameRate = AnimSequence->FrameRate;
        const int32 AnimationLength = AnimSequence->NumFrames;

        TargetKeyFrame = ElapsedTime * static_cast<float>(AnimationFrameRate);
        CurrentKey = static_cast<int32>(TargetKeyFrame) % AnimationLength;
        const int32 NextKey = (CurrentKey + 1) % AnimationLength;
         Alpha = TargetKeyFrame - static_cast<float>(static_cast<int32>(TargetKeyFrame)); // [0 ~ 1]
        
        TMap<int32, FTransform> CurrentFrameTransforms = AnimSequence->Anim[CurrentKey];
        TMap<int32, FTransform> NextFrameTransforms = AnimSequence->Anim[NextKey];

        for (auto& [BoneIdx, CurrentTransform] : CurrentFrameTransforms)
        {
            // 다음 키프레임에 해당 본 데이터가 있는지 확인
            if (NextFrameTransforms.Contains(BoneIdx))
            {
                FTransform NextTransform = NextFrameTransforms[BoneIdx];
                // 두 트랜스폼 사이를 Alpha 비율로 선형 보간
                FTransform InterpolatedTransform = FTransform::Identity;
                InterpolatedTransform.Blend(CurrentTransform, NextTransform, Alpha);
        
                // 보간된 트랜스폼 적용 (로컬 포즈 * 애니메이션 트랜스폼)
                BoneTransforms[BoneIdx] =BoneBindPoseTransforms[BoneIdx] * InterpolatedTransform;
            }
            else
            {
                // 다음 키프레임에 본 데이터가 없으면 현재 트랜스폼만 사용
                BoneTransforms[BoneIdx] = BoneBindPoseTransforms[BoneIdx] * CurrentTransform;
            }
        }
    }
}

void USkeletalMeshComponent::SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset)
{
    SkeletalMeshAsset = InSkeletalMeshAsset;
    AABB = FBoundingBox(SkeletalMeshAsset->GetRenderData()->BoundingBoxMin, SkeletalMeshAsset->GetRenderData()->BoundingBoxMax);
    
    BoneTransforms.Empty();
    BoneBindPoseTransforms.Empty();
    
    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        BoneTransforms.Add(RefSkeleton.RawRefBonePose[i]);
        BoneBindPoseTransforms.Add(RefSkeleton.RawRefBonePose[i]);
    }
}

void USkeletalMeshComponent::GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const
{
    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
    const TArray<FTransform>& BindPose = RefSkeleton.RawRefBonePose; // 로컬
    const int32 BoneNum = RefSkeleton.RawRefBoneInfo.Num();

    // 1. 현재 애니메이션 본 행렬 계산 (계층 구조 적용)
    OutBoneMatrices.Empty();
    OutBoneMatrices.SetNum(BoneNum);

    for (int32 BoneIndex = 0; BoneIndex < BoneNum; ++BoneIndex)
    {
        // 현재 본의 로컬 변환
        FTransform CurrentLocalTransform = BoneTransforms[BoneIndex];
        FMatrix LocalMatrix = CurrentLocalTransform.ToMatrixWithScale(); // FTransform -> FMatrix
        
        // 부모 본의 영향을 적용하여 월드 변환 구성
        int32 ParentIndex = RefSkeleton.RawRefBoneInfo[BoneIndex].ParentIndex;
        if (ParentIndex != INDEX_NONE)
        {
            // 로컬 변환에 부모 월드 변환 적용
            LocalMatrix = LocalMatrix * OutBoneMatrices[ParentIndex];
        }
        
        // 결과 행렬 저장
        OutBoneMatrices[BoneIndex] = LocalMatrix;
    }
}

void USkeletalMeshComponent::SetAnimationEnabled(bool bEnable)
{
    bPlayAnimation = bEnable;

    if (!bPlayAnimation)
    {
        if (SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
        {
            const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
            BoneTransforms = RefSkeleton.RawRefBonePose;
        }
    }
}

int USkeletalMeshComponent::CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const
{
    if (!AABB.Intersect(InRayOrigin, InRayDirection, OutHitDistance))
    {
        return 0;
    }
    if (SkeletalMeshAsset == nullptr)
    {
        return 0;
    }
    
    OutHitDistance = FLT_MAX;
    
    int IntersectionNum = 0;

    const FSkeletalMeshRenderData* RenderData = SkeletalMeshAsset->GetRenderData();

    const TArray<FSkeletalMeshVertex>& Vertices = RenderData->Vertices;
    const int32 VertexNum = Vertices.Num();
    if (VertexNum == 0)
    {
        return 0;
    }
    
    const TArray<UINT>& Indices = RenderData->Indices;
    const int32 IndexNum = Indices.Num();
    const bool bHasIndices = (IndexNum > 0);
    
    int32 TriangleNum = bHasIndices ? (IndexNum / 3) : (VertexNum / 3);
    for (int32 i = 0; i < TriangleNum; i++)
    {
        int32 Idx0 = i * 3;
        int32 Idx1 = i * 3 + 1;
        int32 Idx2 = i * 3 + 2;
        
        if (bHasIndices)
        {
            Idx0 = Indices[Idx0];
            Idx1 = Indices[Idx1];
            Idx2 = Indices[Idx2];
        }

        // 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
        FVector v0 = FVector(Vertices[Idx0].X, Vertices[Idx0].Y, Vertices[Idx0].Z);
        FVector v1 = FVector(Vertices[Idx1].X, Vertices[Idx1].Y, Vertices[Idx1].Z);
        FVector v2 = FVector(Vertices[Idx2].X, Vertices[Idx2].Y, Vertices[Idx2].Z);

        float HitDistance = FLT_MAX;
        if (IntersectRayTriangle(InRayOrigin, InRayDirection, v0, v1, v2, HitDistance))
        {
            OutHitDistance = FMath::Min(HitDistance, OutHitDistance);
            IntersectionNum++;
        }

    }
    return IntersectionNum;
}
