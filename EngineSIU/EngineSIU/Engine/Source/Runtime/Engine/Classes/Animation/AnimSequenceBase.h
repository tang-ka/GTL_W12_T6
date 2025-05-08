#pragma once
#include "AnimationAsset.h"

struct FAnimNotifyEvent;
struct FAnimNotifyTrack;

class UAnimSequenceBase : public UAnimationAsset
{
    DECLARE_CLASS(UAnimSequenceBase, UAnimationAsset)

public:
    UAnimSequenceBase() = default;
    virtual ~UAnimSequenceBase() override = default;

    TArray<FAnimNotifyEvent> Notifies;

    TArray<FAnimNotifyTrack> AnimNotifyTracks;

    float RateScale;

    bool bLoop;

protected:
    float SequenceLength;

public:
    virtual float GetPlayLength() const override;
};
