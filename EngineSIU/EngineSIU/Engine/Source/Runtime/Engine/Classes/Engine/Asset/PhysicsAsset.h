#pragma once
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

class UBodySetup;
class UConstraintSetup;
class USkeletalMesh;
struct FConstraintInstance;

class UPhysicsAsset : public UObject
{
    DECLARE_CLASS(UPhysicsAsset, UObject)

public:
    UPhysicsAsset();
    ~UPhysicsAsset() = default;

    TArray<UBodySetup*>& GetBodySetups() { return BodySetups; }
    TArray<UConstraintSetup*>& GetConstraintSetups() { return ConstraintSetups; }

private:
    TArray<UBodySetup*> BodySetups;
    TArray<UConstraintSetup*> ConstraintSetups;
};

