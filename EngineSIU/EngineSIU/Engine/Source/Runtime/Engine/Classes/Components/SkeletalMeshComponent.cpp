
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

    if (bPlayAnimation && SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton() && AnimSequence)
    {
        const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();

        const int32 AnimationFrameRate = AnimSequence->FrameRate;
        const int32 AnimationLength = AnimSequence->NumFrames;

        const float TargetKeyFrame = ElapsedTime * static_cast<float>(AnimationFrameRate);
        const int32 CurrentKey = static_cast<int32>(TargetKeyFrame) % AnimationLength;
        const int32 NextKey = (CurrentKey + 1) % AnimationLength;
        const float Alpha = TargetKeyFrame - static_cast<float>(static_cast<int32>(TargetKeyFrame)); // [0 ~ 1]
        
        TMap<int32, FTransform> CurrentFrameTransforms = AnimSequence->Anim[CurrentKey];
        TMap<int32, FTransform> NextFrameTransforms = AnimSequence->Anim[NextKey];

        BoneTransforms = RefSkeleton.RawRefBonePose;
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
                BoneTransforms[BoneIdx] = RefSkeleton.RawRefBonePose[BoneIdx] * InterpolatedTransform;
            }
            else
            {
                // 다음 키프레임에 본 데이터가 없으면 현재 트랜스폼만 사용
                BoneTransforms[BoneIdx] = RefSkeleton.RawRefBonePose[BoneIdx] * CurrentTransform;
            }
        }
    }
    else
    {
        const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
        BoneTransforms = RefSkeleton.RawRefBonePose;
    }
}

void USkeletalMeshComponent::SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset)
{
    SkeletalMeshAsset = InSkeletalMeshAsset;

    BoneTransforms.Empty();

    const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();
    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        BoneTransforms.Add(RefSkeleton.RawRefBonePose[i]);
    }
}
