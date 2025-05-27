#include "BodyInstance.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicalMaterial.h"
#include "Components/SceneComponent.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "UObject/Casts.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "World/World.h"

FBodyInstance::FBodyInstance()
{
}

void FBodyInstance::InitBody(USceneComponent* InOwner, UBodySetup* Setup, const FTransform& WorldTransform, const bool bIsStatic)
{
    //UWorld* World = InOwner->GetWorld();
    //if (!World)
    //    return;
    //if (InOwner->GetWorld()->WorldType != EWorldType::None)
    //    return;
    UWorld* World = GEngine->ActiveWorld;
    if (GEngine->ActiveWorld->WorldType != EWorldType::Game && GEngine->ActiveWorld->WorldType != EWorldType::PIE)
        return;
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

    TArray<PxShape*> Shapes;

    for (const FKSphereElem& Sphere : AggGeom.SphereElems)
    {
        PxSphereGeometry SphereGeom(Sphere.Radius);
        PxVec3 SphereCenter(Sphere.Center.X, Sphere.Center.Y, Sphere.Center.Z);
        PxTransform LocalPose(SphereCenter);

        PxShape* Shape = Physics->createShape(SphereGeom, *(Setup->PhysMaterial->GetMaterial()));
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    for (const FKBoxElem& Box : AggGeom.BoxElems)
    {
        PxBoxGeometry BoxGeom((Box.Extent * InOwner->GetComponentScale3D()).ToPxVec3());
        PxVec3 BoxCenter(Box.Center.X, Box.Center.Y, Box.Center.Z);
        FQuat BoxQuat(FRotator(Box.Rotation));
        PxQuat BoxRotation(BoxQuat.X, BoxQuat.Y, BoxQuat.Z, BoxQuat.W);
        PxTransform LocalPose(BoxCenter, BoxRotation);

        PxShape* Shape = Physics->createShape(BoxGeom, *(Setup->PhysMaterial->GetMaterial()));
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    for (const FKSphylElem& Sphyl : AggGeom.SphylElems)
    {
        PxCapsuleGeometry CapsuleGeom(Sphyl.Radius, Sphyl.GetScaledHalfLength(FVector::One()));
        PxVec3 CapsuleCenter(Sphyl.Center.X, Sphyl.Center.Y, Sphyl.Center.Z);
        FQuat CapsuleQuat(FRotator(Sphyl.Rotation));
        PxQuat CapsuleRotation(CapsuleQuat.X, CapsuleQuat.Y, CapsuleQuat.Z, CapsuleQuat.W);
        PxTransform LocalPose(CapsuleCenter, CapsuleRotation);

        PxShape* Shape = Physics->createShape(CapsuleGeom, *(Setup->PhysMaterial->GetMaterial()));
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

        PxShape* Shape = Physics->createShape(ConvexGeom, *(Setup->PhysMaterial->GetMaterial()));
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    Actor = UPhysicsManager::Get().SpawnGameObject(InOwner, PxLocation, PxRotation, Shapes, bIsStatic);  
    if (UStaticMeshComponent* Comp = Cast<UStaticMeshComponent>(InOwner))
    {
        Comp->SetPhysBody(Actor);
        Actor->rigidBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !Comp->IsUseGravity());
        //Delegate 구독
        UPhysicsManager::Get().OnPhysicsContact.AddUObject(Comp, &UStaticMeshComponent::HandlePhysicsContact);
        UPhysicsManager::Get().OnContactPoint.AddUObject(Comp, &UStaticMeshComponent::HandleContactPoint);
    }
    if (USkeletalMeshComponent* Comp = Cast<USkeletalMeshComponent>(InOwner))
    {
        //스켈레탈 메시 처리
    }
} 

void FBodyInstance::TermBody()
{
    if (Actor)
    {
        UPhysicsManager::Get().RemoveGameObject(Actor);
        if (UStaticMeshComponent* Comp = Cast<UStaticMeshComponent>(reinterpret_cast<USceneComponent*>(Actor->rigidBody->userData)))
            Comp->SetPhysBody(nullptr);
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
