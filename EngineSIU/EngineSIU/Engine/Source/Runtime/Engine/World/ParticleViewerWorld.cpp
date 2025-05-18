#include "ParticleViewerWorld.h"

UParticleViewerWorld* UParticleViewerWorld::CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName)
{
    UParticleViewerWorld* NewWorld = FObjectFactory::ConstructObject<UParticleViewerWorld>(InOuter);
    NewWorld->WorldName = InWorldName;
    NewWorld->WorldType = InWorldType;
    NewWorld->InitializeNewWorld();
    
    return NewWorld;
}

void UParticleViewerWorld::Tick(float DeltaTime)
{
    UWorld::Tick(DeltaTime);
}
