#pragma once
#include "CoreUObject/UObject/Object.h"
#include "CoreUObject/UObject/ObjectMacros.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;

class UAnimNotify : public UObject
{
    DECLARE_CLASS(UAnimNotify, UObject)

public:
    UAnimNotify() = default;

    virtual FName GetNotifyName() const { return FName("AnimNotify"); }

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {}
};
