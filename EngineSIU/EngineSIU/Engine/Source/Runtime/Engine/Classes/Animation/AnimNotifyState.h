#pragma once
#include "AnimNotify.h"

class UAnimNotifyState : public UAnimNotify
{
    DECLARE_CLASS(UAnimNotifyState, UAnimNotify)

public:
    UAnimNotifyState() = default;

    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) {}
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) {}
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {}

    virtual FName GetNotifyName() const override { return FName("AnimNotifyState"); }
};
