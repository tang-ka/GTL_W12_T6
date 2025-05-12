#include "AnimSingleNodeInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "AnimationAsset.h"
#include "Animation/AnimNodeBase.h"
#include "Animation/AnimTypes.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Animation/AnimSequence.h"
#include "Misc/FrameTime.h"

UAnimSingleNodeInstance::UAnimSingleNodeInstance()
    : CurrentAsset(nullptr)
    , CurrentTime(0.f)
    , PlayRate(1.f)
    , bLooping(true)
    , bPlaying(true)
    , bReverse(false)
{
}

void UAnimSingleNodeInstance::SetAnimationAsset(UAnimationAsset* NewAsset, bool bIsLooping, float InPlayRate)
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
    CurrentTime = 0.f;
}

void UAnimSingleNodeInstance::NativeInitializeAnimation()
{
}

void UAnimSingleNodeInstance::NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose)
{
    UAnimInstance::NativeUpdateAnimation(DeltaSeconds, OutPose);
#pragma region Anim
    CurrentTime += DeltaSeconds;
    
    UAnimDataModel* DataModel = GetSkelMeshComponent()->GetAnimSequence()->GetDataModel();

    const float TargetKeyFrameLocal = CurrentTime * static_cast<float>( DataModel->GetFrameRate());
    const int32 CurrentFrame = static_cast<int32>(TargetKeyFrameLocal) % (DataModel->GetNumberOfFrames()- 1);
    const float AlphaLocal = TargetKeyFrameLocal - static_cast<float>(static_cast<int32>(TargetKeyFrameLocal)); // [0 ~ 1]

    FFrameTime FrameTime(CurrentFrame, AlphaLocal);
    
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
