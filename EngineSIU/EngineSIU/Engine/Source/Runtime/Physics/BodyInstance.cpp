#include "BodyInstance.h"
#include "PhysicsEngine/BodySetup.h"

FBodyInstance::FBodyInstance()
{
}

void FBodyInstance::InitBody(UBodySetup* Setup, const FTransform& WorldTransform)
{
    PxPhysics* Physics = UPhysicsManager::Get().GetPhysics();
    PxScene* Scene = UPhysicsManager::Get().GetScene();
    PxCooking* Cooking = UPhysicsManager::Get().GetCooking();

    if (!Scene || !Physics || !Cooking)
    {
        return;
    }

    FVector Location = WorldTransform.GetTranslation();
    FQuat Rotation = WorldTransform.GetRotation();
    PxVec3 PxLocation(Location.X, Location.Y, Location.Z);
    PxQuat PxRotation(Rotation.X, Rotation.Y, Rotation.Z, Rotation.W);

    const FKAggregateGeom& AggGeom = Setup->AggGeom;
    PxMaterial* PxMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    TArray<PxShape*> Shapes;

    for (const FKSphereElem& Sphere : AggGeom.SphereElems)
    {
        PxSphereGeometry SphereGeom(Sphere.Radius);
        PxVec3 SphereCenter(Sphere.Center.X, Sphere.Center.Y, Sphere.Center.Z);
        PxTransform LocalPose(SphereCenter);

        PxShape* Shape = Physics->createShape(SphereGeom, *PxMaterial);
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

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

    for (const FKSphylElem& Sphyl : AggGeom.SphylElems)
    {
        PxCapsuleGeometry CapsuleGeom(Sphyl.Radius, Sphyl.HalfHeight);
        PxVec3 CapsuleCenter(Sphyl.Center.X, Sphyl.Center.Y, Sphyl.Center.Z);
        FQuat CapsuleQuat(FRotator(Sphyl.Rotation));
        PxQuat CapsuleRotation(CapsuleQuat.X, CapsuleQuat.Y, CapsuleQuat.Z, CapsuleQuat.W);
        PxTransform LocalPose(CapsuleCenter, CapsuleRotation);

        PxShape* Shape = Physics->createShape(CapsuleGeom, *PxMaterial);
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    for (const FKConvexElem& Convex : AggGeom.ConvexElems)
    {
        PxConvexMeshDesc ConvexDesc;
        ConvexDesc.points.count = Convex.VertexData.Num();
        ConvexDesc.points.stride = sizeof(FVector);
        ConvexDesc.points.data = Convex.VertexData.GetData();
        ConvexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        PxDefaultMemoryOutputStream OutputStream;
        if (!Cooking->cookConvexMesh(ConvexDesc, OutputStream))
        {
            return;
        }
        PxDefaultMemoryInputData InputStream(OutputStream.getData(), OutputStream.getSize());

        PxConvexMesh* ConvexMesh = Physics->createConvexMesh(InputStream);
        PxConvexMeshGeometry ConvexGeom = PxConvexMeshGeometry(ConvexMesh);

        PxVec3 ConvexCenter(Convex.Center.X, Convex.Center.Y, Convex.Center.Z);
        FQuat ConvexQuat(FRotator(Convex.Rotation));
        PxQuat ConvexRotation(ConvexQuat.X, ConvexQuat.Y, ConvexQuat.Z, ConvexQuat.W);
        PxTransform LocalPose(ConvexCenter, ConvexRotation);

        PxShape* Shape = Physics->createShape(ConvexGeom, *PxMaterial);
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    PxMaterial->release();

    Actor = UPhysicsManager::Get().SpawnGameObject(PxLocation, PxRotation, Shapes);
}

void FBodyInstance::TermBody()
{
    if (Actor)
    {
        PxScene* Scene = UPhysicsManager::Get().GetScene();
        if (Scene)
            Scene->removeActor(*(Actor->rigidBody));
        UPhysicsManager::Get().RemoveGameObject(Actor);
        Actor = nullptr;
    }
}

void FBodyInstance::SetBodyTransform(const FTransform& NewTransform)
{
    if (!Actor || !Actor->rigidBody)
    {
        return;
    }

    FVector Location = NewTransform.GetTranslation();
    FQuat Rotation = NewTransform.GetRotation();
    PxVec3 PxLocation(Location.X, Location.Y, Location.Z);
    PxQuat PxRotation(Rotation.X, Rotation.Y, Rotation.Z, Rotation.W);

    Actor->rigidBody->setGlobalPose(PxTransform(PxLocation, PxRotation));
}
