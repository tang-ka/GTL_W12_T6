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
    PxRevoluteJoint* Joints[4] = { nullptr };
    PxRevoluteJoint* SteeringJoint = nullptr;
    float MaxSteerAngle = PxPi / 6.f;
    float DeltaSteerAngle = PxPi / 4.5f;

    //UStaticMeshComponent* BodyComp = nullptr; 바디는 나
    UStaticMeshComponent* WheelComp[4] = { nullptr };

    bool bHasBody = false;

    FVector CarBodyPos = FVector(0, 0, 1.5f);
    FVector BodyExtent = FVector(3.f, 1.f, 0.6f);
    FVector WheelSize = FVector(0.5f);

    const PxVec3 WheelPos[4] =
    {
        {2.5f, 1.5f, 0.9f} , //FR
        {2.5f, -1.5f, 0.9f}, //FL
        {-2.5f, 1.5f, 0.9f}, //RR
        {-2.5f, -1.5f, 0.9f}  //RL
    };

public:
    TSet<EKeys::Type> PressedKeys;
};
