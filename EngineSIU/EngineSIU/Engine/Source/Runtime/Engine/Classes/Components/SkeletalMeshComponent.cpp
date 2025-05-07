
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
    BoneTransforms = BoneBindPoseTransforms;
    ElapsedTime += DeltaTime;
    
    // if ( GetSkeletalMesh() && GetSkeletalMesh()->GetSkeleton() && AnimSequence)
    // {
    //     const FReferenceSkeleton& RefSkeleton = GetSkeletalMesh()->GetSkeleton()->GetReferenceSkeleton();
    //
    //     int32 Key = static_cast<int32>(ElapsedTime * AnimSequence->FrameRate) % AnimSequence->NumFrames;
    //     TMap<int32, FTransform> AnimBoneTransforms = AnimSequence->Anim[Key];
    //
    //     for (auto& [BoneIdx, LocalTransform] : AnimBoneTransforms)
    //     {
    //        BoneTransforms[BoneIdx] = LocalTransform * BoneBindPoseTransforms[BoneIdx];
    //     }
    //
    //     /*
    //     for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    //     {
    //         DotComponents[i]->SetRelativeTransform(MeshComp->BoneTransforms[i]);
    //         DotComponents[i]->SetComponentScale3D(FVector(1.f));
    //     }
    //     */
    // }

}

void USkeletalMeshComponent::SetSkeletalMesh(USkeletalMesh* InSkeletalMesh)
{
    SkeletalMesh = InSkeletalMesh;

    BoneTransforms.Empty();
    BoneBindPoseTransforms.Empty();
    
    const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();

    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        BoneTransforms.Add(RefSkeleton.RawRefBonePose[i]);
        BoneBindPoseTransforms.Add(RefSkeleton.RawRefBonePose[i]);
    }
}

void USkeletalMeshComponent::GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const
{
    const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
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
