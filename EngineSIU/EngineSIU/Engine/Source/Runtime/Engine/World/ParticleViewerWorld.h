#pragma once
#include "World.h"

class UParticleSystemComponent;

class UParticleViewerWorld : public UWorld
{
    DECLARE_CLASS(UParticleViewerWorld, UWorld)
public:
    UParticleViewerWorld() = default;

    static UParticleViewerWorld* CreateWorld(UObject* InOuter, const EWorldType InWorldType = EWorldType::ParticleViewer, const FString& InWorldName = "ParticleViewerWorld");
    
    void SetParticleSystem(UParticleSystemComponent* Component)
    {
        ParticleSystem = Component;
    }

    void Tick(float DeltaTime) override;

private:
    UParticleSystemComponent* ParticleSystem = nullptr;
};
