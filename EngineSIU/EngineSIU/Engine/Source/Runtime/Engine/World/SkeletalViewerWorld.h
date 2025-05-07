#pragma once
#include "World.h"
#include "Components/SkeletalMeshComponent.h"

class USkeletalViewerWorld : public UWorld
{
    DECLARE_CLASS(USkeletalViewerWorld, UWorld)
    
public:
    USkeletalViewerWorld() = default;
    
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

    void Tick(float DeltaTime) override;

private:
    USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
    
};
