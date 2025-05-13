#pragma once

struct FPoseContext;
struct FCompactPose;

struct FAnimationPoseData
{
    FAnimationPoseData(FPoseContext& InPoseContext);
    FAnimationPoseData(FCompactPose& InPose);
	
    FAnimationPoseData() = delete;
    FAnimationPoseData& operator=(const FAnimationPoseData&& Other) = delete;

    const FCompactPose& GetPose() const;
    FCompactPose& GetPose();
    
protected:
    FCompactPose& Pose;
};
