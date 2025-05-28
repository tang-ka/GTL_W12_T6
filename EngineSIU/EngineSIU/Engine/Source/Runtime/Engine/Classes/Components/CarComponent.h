#pragma once

#include "Engine/PhysicsManager.h"
#include "UObject/Casts.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/FObjLoader.h"
#include "Delegates/Delegate.h"

class UCarComponent : public UStaticMeshComponent
{
    DECLARE_CLASS(UCarComponent, UStaticMeshComponent)

public:
    UCarComponent();
    virtual ~UCarComponent() override;

    virtual UObject* Duplicate(UObject* InOuter) override;

    void UpdatePhysics();

    virtual void BeginPlay() override;

    void SpawnComponents();

    void MoveCar();

    void AddPhysBody();

    void RemovePhysBody();

private:
    PxMaterial* DefaultMaterial = nullptr;
    PxRigidDynamic* CarBody = nullptr;
    PxRigidDynamic* Wheels[4] = { nullptr }; //FR, FL, RR, RL
    PxRevoluteJoint* RJoints[2] = { nullptr }; //RR, RL
    PxD6Joint* FJoints[2] = { nullptr }; //FR, FL
    PxRevoluteJoint* SteeringJoint = nullptr;
    float MaxSteerAngle = PxPi / 9.f;
    float DeltaSteerAngle = PxPi / 18.f;

    //UStaticMeshComponent* BodyComp = nullptr; 바디는 나
    UStaticMeshComponent* WheelComp[4] = { nullptr };

    bool bHasBody = false;

    FVector CarBodyPos = FVector(0, 0, 1.5f);
    FVector BodyExtent = FVector(4, 1.25f, 1);
    FVector WheelSize = FVector(1.f);

    const PxVec3 WheelPos[4] =
    {
        {   4.f, 2.5f, 0.5f}, //FR
        {   4.f, -2.5f, 0.5f}, //FL
        {-4.65f, 2.5f, 0.5f}, //RR
        {-4.65f, -2.5f, 0.5f}  //RL
    };

public:
    TSet<EKeys::Type> PressedKeys;
};
