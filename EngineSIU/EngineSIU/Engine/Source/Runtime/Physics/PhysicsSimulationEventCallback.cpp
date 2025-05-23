#include "PhysicsSimulationEventCallback.h"

void FPhysicsSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
}

void FPhysicsSimulationEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
}

void FPhysicsSimulationEventCallback::onWake(PxActor** actors, PxU32 count)
{
}

void FPhysicsSimulationEventCallback::onSleep(PxActor** actors, PxU32 count)
{
}

void FPhysicsSimulationEventCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
{
}
