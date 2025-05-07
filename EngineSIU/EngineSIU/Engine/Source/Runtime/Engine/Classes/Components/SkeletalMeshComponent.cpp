
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
    
    // ElapsedTime += DeltaTime;
    // if ( GetSkeletalMesh() && GetSkeletalMesh()->GetSkeleton() && AnimSequence)
    // {
    //     const FReferenceSkeleton& RefSkeleton = GetSkeletalMesh()->GetSkeleton()->GetReferenceSkeleton();
    //
    //     int32 Key = static_cast<int32>(ElapsedTime * AnimSequence->FrameRate) % AnimSequence->NumFrames;
    //     TMap<int32, FTransform> AnimBoneTransforms = AnimSequence->Anim[Key];
    //
    //     BoneTransforms = RefSkeleton.RawRefBonePose;
    //     for (auto& [BoneIdx, LocalTransform] : AnimBoneTransforms)
    //     {
    //        BoneTransforms[BoneIdx] = LocalTransform * RefSkeleton.RawRefBonePose[BoneIdx];
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

    const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        BoneTransforms.Add(RefSkeleton.RawRefBonePose[i]);
    }
}
