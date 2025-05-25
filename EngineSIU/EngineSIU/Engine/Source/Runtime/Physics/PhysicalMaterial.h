#pragma once
#include "Engine/PhysicsManager.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UPhysicalMaterial : public UObject
{
    DECLARE_CLASS(UPhysicalMaterial, UObject);
public:
    UPhysicalMaterial();
    virtual ~UPhysicalMaterial() = default;

    void SetInfo(float InStaticFric, float InDynamicFric, float InRestitution);
    float GetStaticFriction() { return StaticFriction; }
    float GetDynamicFriction() { return DynamicFriction; }
    float GetRestitution() { return Restitution; }

    PxMaterial* GetMaterial() { return Material; }

private:
    float StaticFriction = 0.5f;
    float DynamicFriction = 0.5f;
    float Restitution = 0.6f;
    PxMaterial* Material = nullptr;
};
