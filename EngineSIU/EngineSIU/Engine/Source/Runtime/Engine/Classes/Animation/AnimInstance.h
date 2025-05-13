#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton;
class USkeletalMeshComponent;
struct FTransform;
struct FPoseContext;

class UAnimInstance : public UObject
{
    DECLARE_CLASS(UAnimInstance, UObject)

public:
    UAnimInstance() = default;

    void InitializeAnimation();

    void UpdateAnimation(float DeltaSeconds, FPoseContext& OutPose);

    virtual void NativeInitializeAnimation();
    
    virtual void NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose);

    virtual USkeletalMeshComponent* GetSkelMeshComponent() const;

    void TriggerAnimNotifies(float DeltaSeconds);

    USkeleton* GetCurrentSkeleton() const { return CurrentSkeleton; }

private:
    USkeleton* CurrentSkeleton;
};
