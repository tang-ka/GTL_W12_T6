#include "MyAnimInstance.h"

#include "Animation/AnimationAsset.h"
#include "Animation/AnimationRuntime.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/FrameTime.h"
#include "Animation/AnimStateMachine.h"
#include "UObject/ObjectFactory.h"

UMyAnimInstance::UMyAnimInstance()
    : CurrentAsset(nullptr)
    , ElapsedTime(0.f)
    , PlayRate(1.f)
    , bLooping(true)
    , bPlaying(true)
    , bReverse(false)
    , PreviousTime(0.f)
    , LoopStartFrame(0)
    , LoopEndFrame(0)
    , CurrentKey(0)
    , AnimA(nullptr)
    , AnimB(nullptr)
{
    StateMachine = FObjectFactory::ConstructObject<UAnimStateMachine>(this);
}

void UMyAnimInstance::SetAnimationAsset(UAnimationAsset* NewAsset, bool bIsLooping, float InPlayRate)
{
    if (NewAsset != CurrentAsset)
    {
        CurrentAsset = NewAsset;
    }

    USkeletalMeshComponent* MeshComponent = GetSkelMeshComponent();
    if (MeshComponent)
    {
        if (MeshComponent->GetSkeletalMeshAsset() == nullptr)
        {
            CurrentAsset = nullptr;
        }
        else if (CurrentAsset != nullptr)
        {
            if (CurrentAsset->GetSkeleton() == nullptr)
            {
                CurrentAsset = nullptr;
            }
        }
    }
    
    bLooping = bIsLooping;
    PlayRate = InPlayRate;
    ElapsedTime = 0.f;
}

void UMyAnimInstance::NativeInitializeAnimation()
{
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose)
{
    UAnimInstance::NativeUpdateAnimation(DeltaSeconds, OutPose);
    StateMachine->ProcessState();
    
#pragma region MyAnim
    USkeletalMeshComponent* SkeletalMeshComp = GetSkelMeshComponent();
    
    if (!AnimA || !AnimB || !SkeletalMeshComp->GetSkeletalMeshAsset() || !SkeletalMeshComp->GetSkeletalMeshAsset()->GetSkeleton())
        return;

    ElapsedTime += DeltaSeconds;
    
    FPoseContext PoseA(this);
    FPoseContext PoseB(this);

    // TODO: FPoseContext의 BoneContainer로 바꾸기
    const FReferenceSkeleton& RefSkeleton = this->GetCurrentSkeleton()->GetReferenceSkeleton();
    PoseA.Pose.InitBones(RefSkeleton.RawRefBoneInfo.Num());
    PoseB.Pose.InitBones(RefSkeleton.RawRefBoneInfo.Num());
    for (int32 BoneIdx = 0; BoneIdx < RefSkeleton.RawRefBoneInfo.Num(); ++BoneIdx)
    {
        PoseA.Pose[BoneIdx] = RefSkeleton.RawRefBonePose[BoneIdx];
        PoseB.Pose[BoneIdx] = RefSkeleton.RawRefBonePose[BoneIdx];
    }
    
    FAnimExtractContext ExtractA(GetElapsedTime(), false);
    FAnimExtractContext ExtractB(GetElapsedTime(), false);

    AnimA->GetAnimationPose(PoseA, ExtractA);
    AnimB->GetAnimationPose(PoseB, ExtractB);

    FAnimationRuntime::BlendTwoPosesTogether(PoseA.Pose, PoseB.Pose, BlendAlpha, OutPose.Pose);
#pragma endregion
}
