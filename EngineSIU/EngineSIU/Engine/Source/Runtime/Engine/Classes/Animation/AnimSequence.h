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
};
