#pragma once
#include "World.h"
#include "Components/SkeletalMeshComponent.h"

class USkeletalViewerWorld : public UWorld
{
public:
    static USkeletalViewerWorld* CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName = "DefaultWorld");

    void SetSkeletalMeshComponent(USkeletalMeshComponent* Component)
    {
        SkeletalMeshComponent = Component;
    }
    USkeletalMeshComponent* GetSkeletalMeshComponent()
    {
        return SkeletalMeshComponent;
    }

    int32 SelectBoneIndex = 0;

private:
    USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
    
};
