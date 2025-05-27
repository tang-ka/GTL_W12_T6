#include "PhysicsSimulationEventCallback.h"
#include "Engine/PhysicsManager.h"

void FPhysicsSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    PxRigidActor* actorA = pairHeader.actors[0];
    PxRigidActor* actorB = pairHeader.actors[1];

    void* dataA = actorA->userData;
    void* dataB = actorB->userData;

    if (dataA && dataB)
    {
        USceneComponent* CompA = reinterpret_cast<USceneComponent*>(dataA);
        USceneComponent* CompB = reinterpret_cast<USceneComponent*>(dataB);
        if (CompA->GetWorld() == CompB->GetWorld())
        {
            UPhysicsManager::Get().OnPhysicsContact.Broadcast(CompA, CompB);
            UE_LOG(ELogLevel::Display, "%s and %s Contact", *CompA->GetOwner()->GetActorLabel(), *CompB->GetOwner()->GetActorLabel());
        }
    }

    for (PxU32 i = 0; i < nbPairs; ++i)
    {
        const PxContactPair& cp = pairs[i];
        // cp.shapes[0], cp.contactCount, cp.flags, etc...
    }
}

void FPhysicsSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    int i = 0;
}

void FPhysicsSimulationEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
    int i = 0;
}

void FPhysicsSimulationEventCallback::onWake(PxActor** actors, PxU32 count)
{
    int i = 0;
}

void FPhysicsSimulationEventCallback::onSleep(PxActor** actors, PxU32 count)
{
    int i = 0;
}

void FPhysicsSimulationEventCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
{
    int i = 0;
}
