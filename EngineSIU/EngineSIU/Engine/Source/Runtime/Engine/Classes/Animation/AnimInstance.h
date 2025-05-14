#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton;
class USkeletalMeshComponent;
class UAnimStateMachine;
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

    USkeletalMeshComponent* GetSkelMeshComponent() const;

    void TriggerAnimNotifies(float DeltaSeconds);

    USkeleton* GetCurrentSkeleton() const { return CurrentSkeleton; }

    UAnimStateMachine* GetStateMachine() const { return StateMachine; }

public:
    UAnimStateMachine* StateMachine;
private:
    USkeleton* CurrentSkeleton;
    
};
