#pragma once

#include "Engine/PhysicsManager.h"

namespace physx
{
    class PxD6Joint;
}
class USkeletalMeshComponent;
class FBodyInstance;
class UConstraintSetup;

class FConstraintInstance
{
public:
    FConstraintInstance();
    ~FConstraintInstance() { TermConstraint(); }

    void InitConstraint(USkeletalMeshComponent* InOwner, UConstraintSetup* Setup);
    void TermConstraint();
    void UpdateConstraint(float DeltaTime);

private:
    USkeletalMeshComponent* OwnerComponent = nullptr;
    
    FBodyInstance* BodyInstanceA = nullptr;
    FBodyInstance* BodyInstanceB = nullptr;

    UConstraintSetup* ConstraintSetup = nullptr;
    
    FTransform TransformInA = FTransform::Identity;
    FTransform TransformInB = FTransform::Identity;

    physx::PxD6Joint* D6Joint = nullptr;
};

