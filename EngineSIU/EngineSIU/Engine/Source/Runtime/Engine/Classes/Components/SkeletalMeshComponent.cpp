
#include "SkeletalMeshComponent.h"

#include "ReferenceSkeleton.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimTypes.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Asset/SkeletalMeshAsset.h"
#include "Misc/FrameTime.h"
#include "UObject/Casts.h"

USkeletalMeshComponent::USkeletalMeshComponent()
{
}

USkeletalMeshComponent::~USkeletalMeshComponent()
{
}

UObject* USkeletalMeshComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->SetSkeletalMeshAsset(SkeletalMeshAsset);
    NewComponent->SetAnimation(AnimSequence);
    NewComponent->SetAnimationEnabled(true);

    return NewComponent;
}

void USkeletalMeshComponent::TickComponent(float DeltaTime)
{
    USkinnedMeshComponent::TickComponent(DeltaTime);

    if (bPlayAnimation)
    {
        ElapsedTime += DeltaTime;
    }
    
    BonePoseTransforms = RefBonePoseTransforms;
    
    if (bPlayAnimation && AnimSequence && SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton())
    {
        const UAnimDataModel* DataModel = AnimSequence->GetDataModel();

        const int32 FrameRate = DataModel->GetFrameRate();
        const int32 NumberOfFrames = DataModel->GetNumberOfFrames();

        const float TargetKeyFrame = ElapsedTime * static_cast<float>(FrameRate);
        const int32 CurrentFrame = static_cast<int32>(TargetKeyFrame) % (NumberOfFrames - 1);
        const float Alpha = TargetKeyFrame - static_cast<float>(static_cast<int32>(TargetKeyFrame)); // [0 ~ 1]

        FFrameTime FrameTime(CurrentFrame, Alpha);
        
        const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();

        // TODO: 인덱스 말고 맵을 통해 FName으로 포즈 계산
        for (int32 BoneIdx = 0; BoneIdx < RefSkeleton.RawRefBoneInfo.Num(); ++BoneIdx)
        {
            FName BoneName = RefSkeleton.RawRefBoneInfo[BoneIdx].Name;
            FTransform RefBoneTransform = RefBonePoseTransforms[BoneIdx];
            BonePoseTransforms[BoneIdx] = RefBoneTransform * DataModel->EvaluateBoneTrackTransform(BoneName, FrameTime, EAnimInterpolationType::Linear);
        }
    }
}

void USkeletalMeshComponent::SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset)
{
    SkeletalMeshAsset = InSkeletalMeshAsset;

    BonePoseTransforms.Empty();
    RefBonePoseTransforms.Empty();
    
    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        BonePoseTransforms.Add(RefSkeleton.RawRefBonePose[i]);
        RefBonePoseTransforms.Add(RefSkeleton.RawRefBonePose[i]);
    }
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
    }
}

void USkeletalMeshComponent::SetAnimation(UAnimSequence* InAnimSequence)
{
    AnimSequence = InAnimSequence;

    SetAnimationEnabled(bPlayAnimation);
    
    ElapsedTime = 0.f;
}
