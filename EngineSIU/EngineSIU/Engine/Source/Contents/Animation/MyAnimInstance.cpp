#include "MyAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimTypes.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Animation/AnimSequence.h"
#include "Engine/SkeletalMesh.h"
#include "Misc/FrameTime.h"
#include "Animation/AnimStateMachine.h"
#include "UObject/ObjectFactory.h"

UMyAnimInstance::UMyAnimInstance()
{
    StateMachine = FObjectFactory::ConstructObject<UAnimStateMachine>(this);
}

void UMyAnimInstance::NativeInitializeAnimation()
{
}

UAnimationAsset* UMyAnimInstance::GetAnimationAsset() const
{
    return GetSkelMeshComponent()->GetAnimSequence();
}

void UMyAnimInstance::SetAnimationAsset(::UAnimationAsset* InAnimationAsset)
{
    if (StateMachine->GetState() == AS_SlowRun)
    {
        
    }
}


void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose)
{
    UAnimInstance::NativeUpdateAnimation(DeltaSeconds, OutPose);
    
#pragma region Anim
    USkeletalMeshComponent* SkeletalMeshComp = GetSkelMeshComponent();
    
    if (!SkeletalMeshComp->GetAnimSequence() || !SkeletalMeshComp->GetSkeletalMeshAsset() || !SkeletalMeshComp->GetSkeletalMeshAsset()->GetSkeleton())
    {
        return;
    }

    UAnimSequence* AnimSequence = SkeletalMeshComp->GetAnimSequence();
    const UAnimDataModel* DataModel = SkeletalMeshComp->GetAnimSequence()->GetDataModel();
    const int32 FrameRate = DataModel->GetFrameRate();
    const int32 NumberOfFrames = DataModel->GetNumberOfFrames();
    
    LoopStartFrame = FMath::Clamp(LoopStartFrame, 0, NumberOfFrames - 2);
    LoopEndFrame = FMath::Clamp(LoopEndFrame, LoopStartFrame + 1, NumberOfFrames - 1);
    const float StartTime = static_cast<float>(LoopStartFrame) / static_cast<float>(FrameRate);
    const float EndTime   = static_cast<float>(LoopEndFrame) / static_cast<float>(FrameRate);

    if (SkeletalMeshComp->bIsAnimationEnabled() && bPlaying)
    {
        float DeltaPlayTime = DeltaSeconds * PlayRate;
        if (bReverse)
        {
            DeltaPlayTime *= -1.0f;
        }

        PreviousTime = ElapsedTime;
        ElapsedTime += DeltaPlayTime;
        
        AnimSequence->EvaluateAnimNotifies(AnimSequence->Notifies, ElapsedTime, PreviousTime, DeltaPlayTime, SkeletalMeshComp, AnimSequence, bLooping);
        
        // 루프 처리
        if (IsLooping())
        {
            if (ElapsedTime > EndTime)
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
            if (!bReverse && ElapsedTime >= EndTime)
            {
                ElapsedTime = StartTime;
                SkeletalMeshComp->DEBUG_SetAnimationEnabled(false);
            }
            else if (bReverse && ElapsedTime <= StartTime)
            {
                ElapsedTime = EndTime;
                SkeletalMeshComp->DEBUG_SetAnimationEnabled(false);
            }
        }
    }

    // 본 트랜스폼 보간
    float TargetKeyFrame = ElapsedTime * static_cast<float>(FrameRate);
    const int32 CurrentFrame = static_cast<int32>(TargetKeyFrame) % (NumberOfFrames - 1);
    float Alpha = TargetKeyFrame - static_cast<float>(CurrentFrame);
    FFrameTime FrameTime(CurrentFrame, Alpha);
    
    CurrentKey = CurrentFrame;
    
    const FReferenceSkeleton& RefSkeleton = GetCurrentSkeleton()->GetReferenceSkeleton();

    // TODO: 인덱스 말고 맵을 통해 FName으로 포즈 계산
    for (int32 BoneIdx = 0; BoneIdx < RefSkeleton.RawRefBoneInfo.Num(); ++BoneIdx)
    {
        FName BoneName = RefSkeleton.RawRefBoneInfo[BoneIdx].Name;
        FTransform RefBoneTransform = RefSkeleton.RawRefBonePose[BoneIdx];
        OutPose.Pose[BoneIdx] = RefBoneTransform * DataModel->EvaluateBoneTrackTransform(BoneName, FrameTime, EAnimInterpolationType::Linear);
    }
#pragma endregion
}

