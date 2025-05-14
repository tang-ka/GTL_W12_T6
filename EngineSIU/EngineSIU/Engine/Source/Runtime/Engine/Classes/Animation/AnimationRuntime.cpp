#include "AnimationRuntime.h"
#include "Animation/AnimationPoseData.h"

template<int32>
void BlendTransform(const FTransform& Source, FTransform& Dest, const float BlendWeight);

template<>
FORCEINLINE void BlendTransform<ETransformBlendMode::Overwrite>(const FTransform& Source, FTransform& Dest, const float BlendWeight)
{
    Dest.SetTranslation(Source.GetTranslation() * BlendWeight);
    Dest.SetRotation(Source.GetRotation() * BlendWeight);
    Dest.SetScale3D(Source.GetScale3D() * BlendWeight);
}

template<>
FORCEINLINE void BlendTransform<ETransformBlendMode::Accumulate>(const FTransform& Source, FTransform& Dest, const float BlendWeight)
{
    Dest.AccumulateWithShortestRotation(Source, BlendWeight);
}

template <int32 TRANSFORM_BLEND_MODE>
FORCEINLINE void BlendPose(const FCompactPose& SourcePose, FCompactPose& ResultPose, const float BlendWeight)
{
    for (int32 BoneIndex = 0; BoneIndex< SourcePose.GetNumBones(); ++BoneIndex)
	{
		BlendTransform<TRANSFORM_BLEND_MODE>(SourcePose[BoneIndex], ResultPose[BoneIndex], BlendWeight);
	}
}

template <>
FORCEINLINE void BlendPose<ETransformBlendMode::Overwrite>(const FCompactPose& SourcePose, FCompactPose& ResultPose, const float BlendWeight)
{
	{
		for (int32 BoneIndex = 0; BoneIndex< SourcePose.GetNumBones(); ++BoneIndex)
		{
			BlendTransform<ETransformBlendMode::Overwrite>(SourcePose[BoneIndex], ResultPose[BoneIndex], BlendWeight);
		}
	}
}

template <>
FORCEINLINE void BlendPose<ETransformBlendMode::Accumulate>(const FCompactPose& SourcePose, FCompactPose& ResultPose, const float BlendWeight)
{
	{
	    for (int32 BoneIndex = 0; BoneIndex< SourcePose.GetNumBones(); ++BoneIndex)
		{
			BlendTransform<ETransformBlendMode::Accumulate>(SourcePose[BoneIndex], ResultPose[BoneIndex], BlendWeight);
		}
	}
}

void FAnimationRuntime::BlendTwoPosesTogether(
    const FCompactPose& SourcePose1,
    const FCompactPose& SourcePose2,
    const float			WeightOfPose1,
    /*out*/ FCompactPose& ResultPose)
{
    FAnimationPoseData AnimationPoseData = { ResultPose };
	
    const FAnimationPoseData SourceOnePoseData(const_cast<FCompactPose&>(SourcePose1));
    const FAnimationPoseData SourceTwoPosedata(const_cast<FCompactPose&>(SourcePose2));

    BlendTwoPosesTogether(SourceOnePoseData, SourceTwoPosedata, WeightOfPose1, AnimationPoseData);
}

void FAnimationRuntime::BlendTwoPosesTogether(const FAnimationPoseData& SourcePoseOneData, const FAnimationPoseData& SourcePoseTwoData, const float WeightOfPoseOne, /*out*/ FAnimationPoseData& OutAnimationPoseData)
{
    FCompactPose& OutPose = OutAnimationPoseData.GetPose();

    const float WeightOfPoseTwo = 1.f - WeightOfPoseOne;

    BlendPose<ETransformBlendMode::Overwrite>(SourcePoseOneData.GetPose(), OutPose, WeightOfPoseOne);
    BlendPose<ETransformBlendMode::Accumulate>(SourcePoseTwoData.GetPose(), OutPose, WeightOfPoseTwo);

    // Ensure that all of the resulting rotations are normalized
    OutPose.NormalizeRotations();
}
