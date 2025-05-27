#include "PhysicsSimulationEventCallback.h"
#include "Engine/PhysicsManager.h"
#include "Components/StaticMeshComponent.h"

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
            for (PxU32 i = 0; i < nbPairs; ++i)
            {
                const PxContactPair& cp = pairs[i];
                // 최대 16개 접촉점까지 복사
                PxContactPairPoint pts[16];
                PxU32 count = cp.extractContacts(pts, 16);
                if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                {
                    /* 새로 닿았을 때 */
                    UPhysicsManager::Get().OnPhysicsContact.Broadcast(CompA, CompB);
                    for (PxU32 j = 0; j < count; ++j)
                    {
                        const PxContactPairPoint& pt = pts[j];
                        // pt.position, pt.normal, pt.impulse, pt.separation
                        FVector Pos(pt.position.x, pt.position.y, pt.position.z);
                        FVector Norm(pt.normal.x, pt.normal.y, pt.normal.z);
                        Cast<UStaticMeshComponent>(CompA)->HandleContactPoint(Pos, Norm);
                    }
                }
                if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
                {
                    /* 계속 닿고 있을 때 */
                }
                if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
                {
                    /* 떨어졌을 때 */
                }

            }
        }
    }
}

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
