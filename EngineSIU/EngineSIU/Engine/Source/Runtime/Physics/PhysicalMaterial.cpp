#include "PhysicalMaterial.h"

UPhysicalMaterial::UPhysicalMaterial()
{
    Material = UPhysicsManager::Get().GetPhysics()->createMaterial(StaticFriction, DynamicFriction, Restitution);
}

void UPhysicalMaterial::SetInfo(float InStaticFric, float InDynamicFric, float InRestitution)
{
    StaticFriction = InStaticFric;
    DynamicFriction = InDynamicFric;
    Restitution = InRestitution;
    Material = UPhysicsManager::Get().GetPhysics()->createMaterial(InStaticFric, InDynamicFric, InRestitution);
}
