
#include "SkeletalMeshComponent.h"

#include "Animation/AnimSequence.h"

USkeletalMeshComponent::USkeletalMeshComponent()
{
    AnimSequence = new UAnimSequence();
}

USkeletalMeshComponent::~USkeletalMeshComponent()
{
    if (AnimSequence)
    {
        delete AnimSequence;
        AnimSequence = nullptr;
    }
}
