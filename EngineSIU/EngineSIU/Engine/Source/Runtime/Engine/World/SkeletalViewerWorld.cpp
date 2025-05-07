#include "SkeletalViewerWorld.h"
#include "Classes/Animation/SkeletalMeshActor.h"
#include "Classes/Engine/AssetManager.h"
USkeletalViewerWorld* USkeletalViewerWorld::CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName)
{
    USkeletalViewerWorld* NewWorld = FObjectFactory::ConstructObject<USkeletalViewerWorld>(InOuter);
    NewWorld->WorldName = InWorldName;
    NewWorld->WorldType = InWorldType;
    NewWorld->InitializeNewWorld();
    
    return NewWorld;
}
