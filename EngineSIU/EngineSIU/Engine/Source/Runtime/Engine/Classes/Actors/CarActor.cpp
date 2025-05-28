#include "CarActor.h"
#include "Engine/PhysicsManager.h"

ACarActor::ACarActor()
{
    UCarComponent* Car = AddComponent<UCarComponent>("Car");
}

UObject* ACarActor::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    
    return NewComponent;
}
