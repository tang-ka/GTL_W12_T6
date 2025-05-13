
#include "AnimSequence.h"

#include "AnimNodeBase.h"
#include "AnimTypes.h"
#include "AnimData/AnimDataModel.h"
#include "Misc/FrameTime.h"

UAnimSequence::UAnimSequence()
{
}

void UAnimSequence::GetAnimationPose(FPoseContext& OutPoseContext, const FAnimExtractContext& ExtractionContext)
{
    GetBonePose(OutPoseContext, ExtractionContext);
}

void UAnimSequence::GetBonePose(FPoseContext& OutPoseContext, const FAnimExtractContext& ExtractionContext)
{
    const UAnimDataModel* DataModel = this->GetDataModel();
    const int32 FrameRate = DataModel->GetFrameRate();
    const int32 NumberOfFrames = DataModel->GetNumberOfFrames();
    
    int32 NumBones = OutPoseContext.Pose.GetNumBones();
    
    float TargetKeyFrame = ExtractionContext.CurrentTime * static_cast<float>(FrameRate);
    const int32 CurrentFrame = static_cast<int32>(TargetKeyFrame) % (NumberOfFrames - 1);
    float Alpha = TargetKeyFrame - static_cast<float>(CurrentFrame);
    FFrameTime FrameTime(CurrentFrame, Alpha);

    for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
    {
        FTransform BoneTransform = OutPoseContext.Pose[BoneIndex];
        OutPoseContext.Pose[BoneIndex] = BoneTransform * DataModel->EvaluateBoneTrackTransform(DataModel->FindBoneTrackByIndex(BoneIndex)->Name, FrameTime, EAnimInterpolationType::Linear);
    }
}

