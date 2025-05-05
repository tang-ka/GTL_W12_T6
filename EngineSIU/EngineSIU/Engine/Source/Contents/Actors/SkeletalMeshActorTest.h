#pragma once
#include "GameFramework/Actor.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;

class ASkeletalMeshActorTest : public AActor
{
    DECLARE_CLASS(ASkeletalMeshActorTest, AActor)

public:
    ASkeletalMeshActorTest();
    virtual ~ASkeletalMeshActorTest() override = default;

    virtual void PostSpawnInitialize() override;
    
    virtual void Tick(float DeltaTime) override;

protected:
    USkeletalMeshComponent* MeshComp;

    float ElapsedTime = 0.f;

    TArray<UStaticMeshComponent*> DotComponents;
};
