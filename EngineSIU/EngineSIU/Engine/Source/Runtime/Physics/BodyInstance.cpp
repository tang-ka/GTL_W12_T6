#include "BodyInstance.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BoxElem.h"

FBodyInstance::FBodyInstance()
{
}

void FBodyInstance::InitBody(UBodySetup* Setup, const FTransform& WorldTransform, bool bIsDynamic)
{
    PxPhysics* Physics = UPhysicsManager::Get().GetPhysics();
    PxScene* Scene = UPhysicsManager::Get().GetScene();
    if (!Scene || !Physics)
        return;

    FVector Location = WorldTransform.GetTranslation();
    FQuat Rotation = WorldTransform.GetRotation();
    PxVec3 PxLocation(Location.X, Location.Y, Location.Z);
    PxQuat PxRotation(Rotation.X, Rotation.Y, Rotation.Z, Rotation.W);
    PxTransform PxWorldTransform = PxTransform(PxLocation, PxRotation);

    if (bIsDynamic)
    {
        Actor = Physics->createRigidDynamic(PxWorldTransform);
    }
    else
    {
        Actor = Physics->createRigidStatic(PxWorldTransform);
    }

    const FKAggregateGeom& AggGeom = Setup->AggGeom;
    PxMaterial* PxMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);
    for (const FKBoxElem& Box : AggGeom.BoxElems)
    {
        PxBoxGeometry BoxGeom(Box.Extent.X, Box.Extent.Y, Box.Extent.Z);
        UE_LOG(ELogLevel::Display, "\nPxBox Added:\nBox Extent: %s\nBox Center: %s", *Box.Extent.ToString(), *Box.Center.ToString());
        PxVec3 BoxCenter(Box.Center.X, Box.Center.Y, Box.Center.Z);
        FQuat BoxQuat(FRotator(Box.Rotation));
        PxQuat BoxRotation(BoxQuat.X, BoxQuat.Y, BoxQuat.Z, BoxQuat.W);
        PxTransform LocalPose(BoxCenter, BoxRotation);

        PxShape* Shape = Physics->createShape(BoxGeom, *PxMaterial);
        Shape->setLocalPose(LocalPose);

        Actor->attachShape(*Shape);
    }

    //for (const FKSphereElem& Sphere : AggGeom.SphereElems)
    //{
    //    PxSphereGeometry SphereGeom(Sphere.Radius);
    //    PxVec3 SphereCenter(Sphere.Center.X, Sphere.Center.Y, Sphere.Center.Z);
    //    PxTransform LocalPose = PxTransform(SphereCenter);
    //
    //    PxShape* Shape = gPhysics->createShape(SphereGeom, *PxMaterial);
    //    Shape->setLocalPose(LocalPose);
    //    Actor->attachShape(*Shape);
    //}

    Scene->addActor(*Actor);
} 

void FBodyInstance::TermBody()
{
    if (Actor)
    {
        PxScene* Scene = UPhysicsManager::Get().GetScene();
        if(Scene)
            Scene->removeActor(*Actor);
        Actor->release();
        Actor = nullptr;
    }
}
