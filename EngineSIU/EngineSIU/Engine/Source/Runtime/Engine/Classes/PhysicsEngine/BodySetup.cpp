#include "BodySetup.h"
#include "PhysicalMaterial.h"

UBodySetup::UBodySetup()
{
    PhysMaterial = new UPhysicalMaterial;
}

UBodySetup::~UBodySetup()
{
    if (PhysMaterial)
        delete PhysMaterial;
}
