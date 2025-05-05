#pragma once
#include "GameFramework/Actor.h"

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
};
