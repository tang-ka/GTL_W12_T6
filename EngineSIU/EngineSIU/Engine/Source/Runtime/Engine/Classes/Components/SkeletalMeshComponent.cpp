
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

    ElapsedTime += DeltaTime;

    if (SkeletalMeshAsset && SkeletalMeshAsset->GetSkeleton() && AnimSequence)
    {
        const FReferenceSkeleton& RefSkeleton = SkeletalMeshAsset->GetSkeleton()->GetReferenceSkeleton();

        int32 Key = static_cast<int32>(ElapsedTime * AnimSequence->FrameRate) % AnimSequence->NumFrames;
        TMap<int32, FTransform> AnimBoneTransforms = AnimSequence->Anim[Key];

        BoneTransforms = RefSkeleton.RawRefBonePose;
        for (auto& [BoneIdx, LocalTransform] : AnimBoneTransforms)
        {
            BoneTransforms[BoneIdx] = LocalTransform * RefSkeleton.RawRefBonePose[BoneIdx];
        }
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
