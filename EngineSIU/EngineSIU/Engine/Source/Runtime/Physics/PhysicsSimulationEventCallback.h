#pragma once

#include <PxPhysicsAPI.h>
#include <PxSimulationEventCallback.h>

using namespace physx;

class FPhysicsSimulationEventCallback : public PxSimulationEventCallback
{
public:
    void onContact(const PxContactPairHeader& pairHeader,
        const PxContactPair* pairs,
        PxU32 nbPairs) override
    {
        PxRigidActor* actorA = pairHeader.actors[0];
        PxRigidActor* actorB = pairHeader.actors[1];

        void* dataA = actorA->userData;
        void* dataB = actorB->userData;

        for (PxU32 i = 0; i < nbPairs; ++i)
        {
            const PxContactPair& cp = pairs[i];
            // cp.shapes[0], cp.contactCount, cp.flags, etc...
        }
    }

    virtual void onTrigger(PxTriggerPair* pairs, PxU32 count) override;
    virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override;
    virtual void onWake(PxActor** actors, PxU32 count) override;
    virtual void onSleep(PxActor** actors, PxU32 count) override;
    virtual void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) override;
};
