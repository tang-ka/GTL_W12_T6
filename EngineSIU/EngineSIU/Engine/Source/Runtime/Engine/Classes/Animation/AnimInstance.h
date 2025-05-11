#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeletalMeshComponent;

class UAnimInstance : public UObject
{
    DECLARE_CLASS(UAnimInstance, UObject)

public:
    UAnimInstance() = default;

    void TriggerAnimNotifies(float DeltaSeconds);

    virtual USkeletalMeshComponent* GetSkelMeshComponent() const;
};
