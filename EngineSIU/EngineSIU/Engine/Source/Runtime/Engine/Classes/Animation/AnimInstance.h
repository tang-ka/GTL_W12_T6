#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton;
class USkeletalMeshComponent;

class UAnimInstance : public UObject
{
    DECLARE_CLASS(UAnimInstance, UObject)

public:
    UAnimInstance() = default;

    void InitializeAnimation();

    void UpdateAnimation(float DeltaSeconds);

    virtual void NativeUpdateAnimation(float DeltaSeconds);

    virtual USkeletalMeshComponent* GetSkelMeshComponent() const;

    void TriggerAnimNotifies(float DeltaSeconds);

    USkeleton* GetCurrentSkeleton() const { return CurrentSkeleton; }

private:
    USkeleton* CurrentSkeleton;
};
