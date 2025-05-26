#pragma once
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

class UBodySetup;
class USkeletalMesh;

class UPhysicsAsset : public UObject
{
    DECLARE_CLASS(UPhysicsAsset, UObject)

public:
    UPhysicsAsset();
    ~UPhysicsAsset() = default;

    TArray<UBodySetup*>& GetBodySetup() { return BodySetups; }
    TArray<FConstraintInstance*> GetConstraintSetups() { return ConstraintSetups; }
    
    void GenerateRagdollFromSkeletalMesh(USkeletalMesh* InSkeletalMesh);

private:
    TArray<UBodySetup*> BodySetups;
    TArray<FConstraintInstance*> ConstraintSetups;
};

