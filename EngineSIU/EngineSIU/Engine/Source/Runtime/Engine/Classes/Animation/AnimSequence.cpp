
#include "AnimSequence.h"

UAnimSequence::UAnimSequence()
{
}

void UAnimSequence::GetAnimationPose(FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext)
{
    GetBonePose(OutAnimationPoseData, ExtractionContext);
}

void UAnimSequence::GetBonePose(FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext)
{
}

void UAnimSequence::SerializeAsset(FArchive& Ar)
{
    Super::SerializeAsset(Ar);
}
