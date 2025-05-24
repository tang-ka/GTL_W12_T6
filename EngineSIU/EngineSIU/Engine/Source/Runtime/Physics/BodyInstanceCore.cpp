#include "BodyInstanceCore.h"
#include "PhysicsEngine/BodySetupCore.h"

FBodyInstanceCore::FBodyInstanceCore()
    : bSimulatePhysics(false)
    , bOverrideMass(false)
    , bEnableGravity(true)
    , bUpdateKinematicFromSimulation(false)
    , bAutoWeld(false)
    , bStartAwake(true)
    , bGenerateWakeEvents(false)
    , bDirtyMassProps(false)
{
}

bool FBodyInstanceCore::ShouldInstanceSimulatingPhysics() const
{
    return bSimulatePhysics && BodySetup && BodySetup->GetCollisionTraceFlag() != ECollisionTraceFlag::CTF_UseComplexAsSimple;
}
