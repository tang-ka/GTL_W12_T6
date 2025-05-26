#pragma once

#include <PxPhysicsAPI.h>
#include <PxSimulationEventCallback.h>
#include <Components/SceneComponent.h>
#include <GameFramework/Actor.h>

using namespace physx;

class FPhysicsSimulationEventCallback : public PxSimulationEventCallback
{
public:
    void onContact(const PxContactPairHeader& pairHeader,
        const PxContactPair* pairs,
        PxU32 nbPairs) override;

    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
    virtual void onWake(PxActor** actors, PxU32 count) override;
    virtual void onSleep(PxActor** actors, PxU32 count) override;
    virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override;
};
