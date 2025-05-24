#include "PhysicsViewerWorld.h"
#include "Classes/Engine/AssetManager.h"
#include "Engine/EditorEngine.h"

UPhysicsViewerWorld* UPhysicsViewerWorld::CreateWorld(UObject* InOuter, const EWorldType InWorldType, const FString& InWorldName)
{
    UPhysicsViewerWorld* NewWorld = FObjectFactory::ConstructObject<UPhysicsViewerWorld>(InOuter);
    NewWorld->WorldName = InWorldName;
    NewWorld->WorldType = InWorldType;
    NewWorld->InitializeNewWorld();
    NewWorld->SelectBoneIndex = 0;

    return NewWorld;
}

void UPhysicsViewerWorld::Tick(float DeltaTime)
{
    UWorld::Tick(DeltaTime);

    //TODO: 임시로 SkeletalMeshComponent을 강제로 셀렉트 함
    Cast<UEditorEngine>(GEngine)->SelectActor(SkeletalMeshComponent->GetOwner());
    Cast<UEditorEngine>(GEngine)->SelectComponent(SkeletalMeshComponent);
}
