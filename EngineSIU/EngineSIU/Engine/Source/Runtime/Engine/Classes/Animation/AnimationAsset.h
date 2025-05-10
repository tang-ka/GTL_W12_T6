#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton;

class UAnimationAsset : public UObject
{
    DECLARE_CLASS(UAnimationAsset, UObject)

public:
    UAnimationAsset() = default;
    virtual ~UAnimationAsset() override = default;

    virtual float GetPlayLength() const { return 0.f; }

    void SetSkeleton(USkeleton* NewSkeleton) { Skeleton = NewSkeleton; }

    USkeleton* GetSkeleton() const { return Skeleton; }

private:
    USkeleton* Skeleton;
};
