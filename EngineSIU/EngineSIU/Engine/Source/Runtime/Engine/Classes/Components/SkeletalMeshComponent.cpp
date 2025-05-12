
#include "SkeletalMeshComponent.h"

#include "ReferenceSkeleton.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimTypes.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Asset/SkeletalMeshAsset.h"
#include "Misc/FrameTime.h"
#include "Animation/AnimNotifyState.h"

bool USkeletalMeshComponent::bCPUSkinning = false;

USkeletalMeshComponent::USkeletalMeshComponent()
{
    AnimSequence = new UAnimSequence();
    CPURenderData = std::make_unique<FSkeletalMeshRenderData>();
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

    if (!AnimSequence || !SkeletalMeshAsset || !SkeletalMeshAsset->GetSkeleton())
        return;

    const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
    const int32 FrameRate = DataModel->GetFrameRate();
    const int32 NumberOfFrames = DataModel->GetNumberOfFrames();
    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();

    
    LoopStartFrame = FMath::Clamp(LoopStartFrame, 0, NumberOfFrames - 2);
    LoopEndFrame = FMath::Clamp(LoopEndFrame, LoopStartFrame + 1, NumberOfFrames - 1);
    const float StartTime = static_cast<float>(LoopStartFrame) / static_cast<float>(FrameRate);
    const float EndTime   = static_cast<float>(LoopEndFrame) / static_cast<float>(FrameRate)

    ;

    if (bPlayAnimation && !bPuaseAnimation)
    {
        float DeltaPlayTime = DeltaTime * PlaySpeed;
        if (bPlayReverse)
        {
            DeltaPlayTime *= -1.0f;
        }

        ElapsedTime += DeltaPlayTime;

        // 루프 처리
        if (bPlayLooping)
        {
            if (ElapsedTime >= EndTime)
            {
                ElapsedTime = StartTime + FMath::Fmod(ElapsedTime - StartTime, EndTime - StartTime);
            }
            else if (ElapsedTime <= StartTime)
            {
                ElapsedTime = EndTime - FMath::Fmod(EndTime - ElapsedTime, EndTime - StartTime);
            }
        }
        else
        {
            if (!bPlayReverse && ElapsedTime >= EndTime)
            {
                ElapsedTime = StartTime;
                bPlayAnimation = false;
            }
            else if (bPlayReverse && ElapsedTime <= StartTime)
            {
                ElapsedTime = EndTime;
                bPlayAnimation = false;
            }
        }
    }

    // 포즈 초기화
    BonePoseTransforms = RefBonePoseTransforms;

    // 본 트랜스폼 보간
    TargetKeyFrame = ElapsedTime * static_cast<float>(FrameRate);
    const int32 CurrentFrame = static_cast<int32>(TargetKeyFrame) % (NumberOfFrames - 1);
    Alpha = TargetKeyFrame - static_cast<float>(CurrentFrame);
    FFrameTime FrameTime(CurrentFrame, Alpha);


    CurrentKey = CurrentFrame;

    for (int32 BoneIdx = 0; BoneIdx < RefSkeleton.RawRefBoneInfo.Num(); ++BoneIdx)
    {
        FName BoneName = RefSkeleton.RawRefBoneInfo[BoneIdx].Name;
        FTransform RefBoneTransform = RefBonePoseTransforms[BoneIdx];
        BonePoseTransforms[BoneIdx] = RefBoneTransform * DataModel->EvaluateBoneTrackTransform(BoneName, FrameTime, EAnimInterpolationType::Linear);
    }

    if (bCPUSkinning)
    {
        TArray<FMatrix> CurrentGlobalBoneMatrices;
        GetCurrentGlobalBoneMatrices(CurrentGlobalBoneMatrices);
        const int32 BoneNum = RefSkeleton.RawRefBoneInfo.Num();

        TArray<FMatrix> FinalBoneMatrices;
        FinalBoneMatrices.SetNum(BoneNum);
        for (int32 BoneIndex = 0; BoneIndex < BoneNum; ++BoneIndex)
        {
            FinalBoneMatrices[BoneIndex] = RefSkeleton.InverseBindPoseMatrices[BoneIndex] * CurrentGlobalBoneMatrices[BoneIndex];
        }

        const FSkeletalMeshRenderData* RenderData = SkeletalMeshAsset->GetRenderData();

        for (int i = 0; i < RenderData->Vertices.Num(); i++)
        {
            const FSkeletalMeshVertex& Vertex = RenderData->Vertices[i];
            float TotalWeight = 0.0f;
            FVector SkinnedPosition(0.0f), SkinnedNormal(0.0f);

            for (int j = 0; j < 4; ++j)
            {
                float Weight = Vertex.BoneWeights[j];
                TotalWeight += Weight;
                if (Weight > 0.0f)
                {
                    uint32 BoneIdx = Vertex.BoneIndices[j];
                    FVector pos = FinalBoneMatrices[BoneIdx].TransformPosition(FVector(Vertex.X, Vertex.Y, Vertex.Z));
                    FVector4 norm4 = FinalBoneMatrices[BoneIdx].TransformFVector4(FVector4(Vertex.NormalX, Vertex.NormalY, Vertex.NormalZ, 0.0f));
                    FVector norm(norm4.X, norm4.Y, norm4.Z);
                    SkinnedPosition += pos * Weight;
                    SkinnedNormal += norm * Weight;
                }
            }

            if (TotalWeight < 0.001f)
            {
                SkinnedPosition = FVector(Vertex.X, Vertex.Y, Vertex.Z);
                SkinnedNormal = FVector(Vertex.NormalX, Vertex.NormalY, Vertex.NormalZ);
            }
            else if (FMath::Abs(TotalWeight - 1.0f) > 0.001f)
            {
                SkinnedPosition /= TotalWeight;
                SkinnedNormal /= TotalWeight;
            }

            CPURenderData->Vertices[i].X = SkinnedPosition.X;
            CPURenderData->Vertices[i].Y = SkinnedPosition.Y;
            CPURenderData->Vertices[i].Z = SkinnedPosition.Z;
            CPURenderData->Vertices[i].NormalX = SkinnedNormal.X;
            CPURenderData->Vertices[i].NormalY = SkinnedNormal.Y;
            CPURenderData->Vertices[i].NormalZ = SkinnedNormal.Z;
        }
    }
}



void USkeletalMeshComponent::SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset)
{
    SkeletalMeshAsset = InSkeletalMeshAsset;

    BonePoseTransforms.Empty();
    RefBonePoseTransforms.Empty();
    AABB = FBoundingBox(InSkeletalMeshAsset->GetRenderData()->BoundingBoxMin, SkeletalMeshAsset->GetRenderData()->BoundingBoxMax);
    
    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        BonePoseTransforms.Add(RefSkeleton.RawRefBonePose[i]);
        RefBonePoseTransforms.Add(RefSkeleton.RawRefBonePose[i]);
    }
    
    CPURenderData->Vertices = InSkeletalMeshAsset->GetRenderData()->Vertices;
    CPURenderData->Indices = InSkeletalMeshAsset->GetRenderData()->Indices;
    CPURenderData->ObjectName = InSkeletalMeshAsset->GetRenderData()->ObjectName;
    CPURenderData->MaterialSubsets = InSkeletalMeshAsset->GetRenderData()->MaterialSubsets;
}

void USkeletalMeshComponent::GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const
{
    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
    const int32 BoneNum = RefSkeleton.RawRefBoneInfo.Num();

    // 1. 현재 애니메이션 본 행렬 계산 (계층 구조 적용)
    OutBoneMatrices.Empty();
    OutBoneMatrices.SetNum(BoneNum);

    for (int32 BoneIndex = 0; BoneIndex < BoneNum; ++BoneIndex)
    {
        // 현재 본의 로컬 변환
        FTransform CurrentLocalTransform = BonePoseTransforms[BoneIndex];
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
    bPlayAnimation = AnimSequence && bEnable;
    
    if (!bPlayAnimation)
    {
        if (SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
        {
            const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
            BonePoseTransforms = RefSkeleton.RawRefBonePose;
        }
        ElapsedTime = 0.f;
        CPURenderData->Vertices = SkeletalMeshAsset->GetRenderData()->Vertices;
        CPURenderData->Indices = SkeletalMeshAsset->GetRenderData()->Indices;
        CPURenderData->ObjectName = SkeletalMeshAsset->GetRenderData()->ObjectName;
        CPURenderData->MaterialSubsets = SkeletalMeshAsset->GetRenderData()->MaterialSubsets;
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

const FSkeletalMeshRenderData* USkeletalMeshComponent::GetCPURenderData() const
{
    return CPURenderData.get();
}

void USkeletalMeshComponent::SetCPUSkinning(bool flag)
{
    bCPUSkinning = flag;
}

bool USkeletalMeshComponent::GetCPUSkinning()
{
    return bCPUSkinning;
}

void USkeletalMeshComponent::SetAnimation(UAnimSequence* InAnimSequence)
{
    AnimSequence = InAnimSequence;

    SetAnimationEnabled(bPlayAnimation);
    
    if (AnimSequence && SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
    {
        const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
        const int32 FrameRate = DataModel->GetFrameRate();

        const float StartTime = static_cast<float>(LoopStartFrame) / FrameRate;
        const float EndTime   = static_cast<float>(LoopEndFrame) / FrameRate;

        ElapsedTime = bPlayReverse ? EndTime : StartTime;
    }
    else
    {
        ElapsedTime = 0.f;
    }
}

float USkeletalMeshComponent::GetPlaySpeed() const
{
    return PlaySpeed;
}
void USkeletalMeshComponent::SetPlaySpeed(float InSpeed)
{
    PlaySpeed = InSpeed;
}

int32 USkeletalMeshComponent::GetLoopStartFrame() const
{
    return LoopStartFrame;
}
void USkeletalMeshComponent::SetLoopStartFrame(int32 InStart)
{
    LoopStartFrame = InStart;
}

int32 USkeletalMeshComponent::GetLoopEndFrame() const
{
    return LoopEndFrame;
}
void USkeletalMeshComponent::SetLoopEndFrame(int32 InEnd)
{
    LoopEndFrame = InEnd;
}

bool USkeletalMeshComponent::IsPlayReverse() const
{
    return bPlayReverse;
}
void USkeletalMeshComponent::SetPlayReverse(bool bEnable)
{
    bPlayReverse = bEnable;

    if (bEnable && AnimSequence && SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
    {
        const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
        const int32 FrameRate = DataModel->GetFrameRate();
        const float EndTime = static_cast<float>(LoopEndFrame) / static_cast<float>(FrameRate);

        ElapsedTime = EndTime;  
    }
    else if (!bEnable && AnimSequence && SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
    {
        const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
        const int32 FrameRate = DataModel->GetFrameRate();
        const float StartTime = static_cast<float>(LoopStartFrame) / static_cast<float>(FrameRate);

        ElapsedTime = StartTime; 
    }
}


bool USkeletalMeshComponent::IsPaused() const
{
    return bPuaseAnimation;
}
void USkeletalMeshComponent::SetPaused(bool bPause)
{
    bPuaseAnimation = bPause;
}

bool USkeletalMeshComponent::IsLooping() const
{
    return bPlayLooping;
}
void USkeletalMeshComponent::SetLooping(bool bEnable)
{
    bPlayLooping = bEnable;
}

static void EvaluateAnimNotifies(const TArray<FAnimNotifyEvent>& Notifies, float CurrentTime, float PreviousTime, float DeltaTime, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* AnimAsset, bool bIsLooping)
{
    for (FAnimNotifyEvent& NotifyEvent : const_cast<TArray<FAnimNotifyEvent>&>(Notifies))
    {
        const float StartTime = NotifyEvent.Time;
        const float EndTime = NotifyEvent.GetEndTime();
        const bool bReversed = DeltaTime < 0.0f;

        const bool bPassed = bReversed
            ? (PreviousTime >= StartTime && CurrentTime < StartTime)
            : (PreviousTime <= StartTime && CurrentTime > StartTime);

        const bool bInside = CurrentTime >= StartTime && CurrentTime < EndTime;

        if (!NotifyEvent.IsState()) 
        {
            if (bPassed || (bIsLooping && !bReversed && PreviousTime > CurrentTime && StartTime >= 0.f && StartTime < CurrentTime))
            {
                if (NotifyEvent.Notify)
                {
                    NotifyEvent.Notify->Notify(MeshComp, AnimAsset);
                }
                NotifyEvent.bTriggered = true;
            }
        }
        else 
        {
            if (bInside && !NotifyEvent.bStateActive)
            {
                if (NotifyEvent.NotifyState)
                {
                    NotifyEvent.NotifyState->NotifyBegin(MeshComp, AnimAsset, NotifyEvent.Duration);
                }
                NotifyEvent.bStateActive = true;
            }
            else if (bInside && NotifyEvent.bStateActive)
            {
                if (NotifyEvent.NotifyState)
                {
                    NotifyEvent.NotifyState->NotifyTick(MeshComp, AnimAsset, DeltaTime);
                }
            }
            else if (!bInside && NotifyEvent.bStateActive)
            {
                if (NotifyEvent.NotifyState)
                {
                    NotifyEvent.NotifyState->NotifyEnd(MeshComp, AnimAsset);
                }
                NotifyEvent.bStateActive = false;
            }
        }
    }
}
