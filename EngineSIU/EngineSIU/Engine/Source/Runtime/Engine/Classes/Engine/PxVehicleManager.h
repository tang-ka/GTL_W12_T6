#pragma once
#include "PhysicsManager.h"
#include "GameFramework/Actor.h"

class UPxVehicleManager : public UObject
{
    DECLARE_CLASS(UPxVehicleManager, UObject)

public:
    UPxVehicleManager() = default;
    ~UPxVehicleManager() = default;

    void Initialize(PxPhysics* InPhysics, PxScene* InScene);

    void Tick(float DeltaTime);

    void AddActors();

    bool CanPlay() { return bHasActor; }

private:
    PxPhysics* Physics = nullptr;
    PxScene* Scene = nullptr;
    PxMaterial* DefaultMaterial = nullptr;
    PxRigidDynamic* CarBody = nullptr;
    PxRigidDynamic* Wheels[4] = { nullptr };

    AActor* BodyActor = nullptr;
    AActor* WheelActors[4] = { nullptr };

    bool bHasActor = false;

    const PxVec3 WheelPos[4] =
    {
        {1.f, 0.9f, 0.7f} , //FR
        {1.f, -0.9f, 0.7f}, //FL
        {-1.f, 0.9f, 0.7f}, //RR
        {-1.f, -0.9f, 0.7f}  //RL
    };
};
