#pragma once
#include "AnimSequenceBase.h"
#include "UObject/ObjectMacros.h"

struct FTransform;
struct FBoneAnimationTrack;
struct FAnimationPoseData;
struct FAnimExtractContext;

class UAnimSequence : public UAnimSequenceBase
{
    DECLARE_CLASS(UAnimSequence, UAnimSequenceBase)

public:
    UAnimSequence();
    virtual ~UAnimSequence() override = default;
    
    virtual void GetAnimationPose(FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext);
    
    void GetBonePose(FAnimationPoseData& OutAnimationPoseData, const FAnimExtractContext& ExtractionContext);

    virtual void SerializeAsset(FArchive& Ar) override;
};
