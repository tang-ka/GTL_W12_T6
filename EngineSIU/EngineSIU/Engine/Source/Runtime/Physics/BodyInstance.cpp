#include "BodyInstance.h"
#include "PhysicsEngine/BodySetup.h"

FBodyInstance::FBodyInstance()
{
}

void FBodyInstance::InitBody(UBodySetup* Setup, const FTransform& WorldTransform)
{
    PxPhysics* Physics = UPhysicsManager::Get().GetPhysics();
    PxScene* Scene = UPhysicsManager::Get().GetScene();
    if (!Scene || !Physics)
        return;

    FVector Location = WorldTransform.GetTranslation();
    FQuat Rotation = WorldTransform.GetRotation();
    PxVec3 PxLocation(Location.X, Location.Y, Location.Z);
    PxQuat PxRotation(Rotation.X, Rotation.Y, Rotation.Z, Rotation.W);

    const FKAggregateGeom& AggGeom = Setup->AggGeom;
    PxMaterial* PxMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    TArray<PxShape*> Shapes;

    for (const FKBoxElem& Box : AggGeom.BoxElems)
    {
        PxBoxGeometry BoxGeom(Box.Extent.X, Box.Extent.Y, Box.Extent.Z);
        PxVec3 BoxCenter(Box.Center.X, Box.Center.Y, Box.Center.Z);
        FQuat BoxQuat(FRotator(Box.Rotation));
        PxQuat BoxRotation(BoxQuat.X, BoxQuat.Y, BoxQuat.Z, BoxQuat.W);
        PxTransform LocalPose(BoxCenter, BoxRotation);

        PxShape* Shape = Physics->createShape(BoxGeom, *PxMaterial);
        Shape->setLocalPose(LocalPose);

        Shapes.Add(Shape);
    }

    //for (const FKSphereElem& Sphere : AggGeom.SphereElems)
    //{
    //    PxSphereGeometry SphereGeom(Sphere.Radius);
    //    PxVec3 SphereCenter(Sphere.Center.X, Sphere.Center.Y, Sphere.Center.Z);
    //    PxTransform LocalPose = PxTransform(SphereCenter);
    //
    //    PxShape* Shape = gPhysics->createShape(SphereGeom, *PxMaterial);
    //    Shape->setLocalPose(LocalPose);
    //    Shapes.Add(Shape);
    //}
    PxMaterial->release();

    Actor = UPhysicsManager::Get().SpawnGameObject(PxLocation, PxRotation, Shapes);
} 

void FBodyInstance::TermBody()
{
    if (Actor)
    {
        PxScene* Scene = UPhysicsManager::Get().GetScene();
        if(Scene)
            Scene->removeActor(*(Actor->rigidBody));
        UPhysicsManager::Get().RemoveGameObject(Actor);
        Actor = nullptr;
    }
}
