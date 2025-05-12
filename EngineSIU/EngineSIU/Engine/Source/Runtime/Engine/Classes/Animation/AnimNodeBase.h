#pragma once
#include "BonePose.h"

class UAnimInstance;

struct FAnimationBaseContext
{
    UAnimInstance* AnimInstance;

    FAnimationBaseContext(UAnimInstance* InAnimInstance);
};

/** Evaluation context passed around during animation tree evaluation */
struct FPoseContext : public FAnimationBaseContext
{
    FCompactPose Pose;
    // FBlendedCurve Curve;

    FPoseContext(UAnimInstance* InAnimInstance)
        : FAnimationBaseContext(InAnimInstance)
    {}
};
