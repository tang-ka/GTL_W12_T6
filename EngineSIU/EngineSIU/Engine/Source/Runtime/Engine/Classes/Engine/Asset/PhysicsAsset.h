#pragma once
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

class UBodySetup;

class UPhysicsAsset : public UObject
{
    DECLARE_CLASS(UPhysicsAsset, UObject)

public:
    UPhysicsAsset();
    ~UPhysicsAsset() = default;

    TArray<UBodySetup*> GetBodySetup() { return BodySetups; }

private:
    TArray<UBodySetup*> BodySetups;

};

