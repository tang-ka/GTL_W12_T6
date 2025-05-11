#include "AnimSingleNodeInstance.h"

#include "AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"

UAnimSingleNodeInstance::UAnimSingleNodeInstance()
    : CurrentAsset(nullptr)
    , CurrentTime(0.f)
    , PlayRate(1.f)
    , bLooping(true)
    , bPlaying(true)
    , bReverse(false)
{
}

void UAnimSingleNodeInstance::SetAnimationAsset(UAnimationAsset* NewAsset, bool bIsLooping, float InPlayRate)
{
    if (NewAsset != CurrentAsset)
    {
        CurrentAsset = NewAsset;
    }

    USkeletalMeshComponent* MeshComponent = GetSkelMeshComponent();
    if (MeshComponent)
    {
        if (MeshComponent->GetSkeletalMeshAsset() == nullptr)
        {
            CurrentAsset = nullptr;
        }
        else if (CurrentAsset != nullptr)
        {
            if (CurrentAsset->GetSkeleton() == nullptr)
            {
                CurrentAsset = nullptr;
            }
        }
    }
    
    bLooping = bIsLooping;
    PlayRate = InPlayRate;
    CurrentTime = 0.f;
}
