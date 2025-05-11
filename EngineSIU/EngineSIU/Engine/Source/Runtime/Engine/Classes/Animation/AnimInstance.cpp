#include "AnimInstance.h"

#include "UObject/Casts.h"

USkeletalMeshComponent* UAnimInstance::GetSkelMeshComponent() const
{
    return Cast<USkeletalMeshComponent>(GetOuter());
}
