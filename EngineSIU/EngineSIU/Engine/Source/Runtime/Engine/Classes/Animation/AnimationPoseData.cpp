#include "AnimationPoseData.h"

#include "AnimNodeBase.h"

FAnimationPoseData::FAnimationPoseData(FPoseContext& InPoseContext)
    : Pose(InPoseContext.Pose)
{
}

FAnimationPoseData::FAnimationPoseData(FCompactPose& InPose)
    : Pose(InPose)
{
}

const FCompactPose& FAnimationPoseData::GetPose() const
{
    return Pose;
}

FCompactPose& FAnimationPoseData::GetPose()
{
    return Pose;
}
