#include "AnimInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/Casts.h"

void UAnimInstance::InitializeAnimation()
{
    USkeletalMeshComponent* OwnerComponent = GetSkelMeshComponent();
    if (OwnerComponent->GetSkeletalMeshAsset() != nullptr)
    {
        CurrentSkeleton = OwnerComponent->GetSkeletalMeshAsset()->GetSkeleton();
    }
    else
    {
        CurrentSkeleton = nullptr;
    }
}

void UAnimInstance::UpdateAnimation(float DeltaSeconds, FPoseContext& OutPose)
{
    NativeUpdateAnimation(DeltaSeconds, OutPose);
}

void UAnimInstance::NativeInitializeAnimation()
{
}

void UAnimInstance::NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose)
{
}

void UAnimInstance::SetAnimState(EAnimState InAnimState)
{
}

UAnimStateMachine* UAnimInstance::GetStateMachine() const
{
    return nullptr;
}

UAnimSequence* UAnimInstance::GetCurrAnim() const
{
    return nullptr;
}

UAnimSequence* UAnimInstance::GetPrevAnim() const
{
    return nullptr;
}

float UAnimInstance::GetBlendDuration() const
{
    return 0.f;
}

void UAnimInstance::SetBlendDuration(float InBlendDuration)
{
}

USkeletalMeshComponent* UAnimInstance::GetSkelMeshComponent() const
{
    return Cast<USkeletalMeshComponent>(GetOuter());
}

void UAnimInstance::TriggerAnimNotifies(float DeltaSeconds)
{
}
