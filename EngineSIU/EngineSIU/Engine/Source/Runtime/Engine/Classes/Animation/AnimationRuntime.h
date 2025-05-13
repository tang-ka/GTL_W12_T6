#pragma once

#include "BonePose.h"

struct FAnimationPoseData;

namespace ETransformBlendMode
{
enum Type
{
    Overwrite,
    Accumulate
};
}

class FAnimationRuntime
{
public:
    static void BlendTwoPosesTogether(
        const FCompactPose& SourcePose1,
        const FCompactPose& SourcePose2,
        const float WeightOfPose1,
        /*out*/ FCompactPose& ResultPose);

    static void BlendTwoPosesTogether(
    const FAnimationPoseData& SourcePoseOneData,
    const FAnimationPoseData& SourcePoseTwoData,
    const float WeightOfPoseOne,
    /*out*/ FAnimationPoseData& OutAnimationPoseData);
};
