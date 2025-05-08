#pragma once
#include "AnimSequenceBase.h"
#include "UObject/ObjectMacros.h"

struct FTransform;
struct FBoneAnimationTrack;

class UAnimSequence : public UAnimSequenceBase
{
    DECLARE_CLASS(UAnimSequence, UAnimSequenceBase)

public:
    UAnimSequence();
    virtual ~UAnimSequence() override = default;

    // All individual bone animation tracks
    TArray<FBoneAnimationTrack> BoneAnimationTracks;

    // Rate at which the animated data is sampled
    int32 FrameRate = 30;

    // Total number of sampled animated frames
    int32 NumberOfFrames = 240;

    // Total number of sampled animated keys
    int32 NumberOfKeys;
};
