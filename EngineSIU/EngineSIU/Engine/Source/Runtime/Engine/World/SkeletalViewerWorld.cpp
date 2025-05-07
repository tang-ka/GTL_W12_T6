#include "SkeletalViewerWorld.h"
#include "Classes/Animation/SkeletalMeshActor.h"
#include "Classes/Engine/AssetManager.h"
USkeletalViewerWorld* USkeletalViewerWorld::CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName)
{
    USkeletalViewerWorld* NewWorld = FObjectFactory::ConstructObject<USkeletalViewerWorld>(InOuter);
    NewWorld->WorldName = InWorldName;
    NewWorld->WorldType = InWorldType;
    NewWorld->InitializeNewWorld();

    ASkeletalMeshActor* SkeletalActor = NewWorld->SpawnActor<ASkeletalMeshActor>();
    SkeletalActor->SetActorTickInEditor(true);
    USkeletalMeshComponent* MeshComp = SkeletalActor->AddComponent<USkeletalMeshComponent>();
    SkeletalActor->SetRootComponent(MeshComp);
    SkeletalActor->SetActorLabel(TEXT("OBJ_SKELETALMESH"));
    MeshComp->SetSkeletalMesh(UAssetManager::Get().GetSkeletalMesh("Contents/test"));
    NewWorld->SetSkeletalMeshComponent(MeshComp);
    
    return NewWorld;
}
