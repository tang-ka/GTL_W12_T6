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
    UWorld* World = GEngine->ActiveWorld;
    if (GEngine->ActiveWorld->WorldType != EWorldType::Game && 
        GEngine->ActiveWorld->WorldType != EWorldType::PIE)
    {
        return;
    }

    PxPhysics* Physics = UPhysicsManager::Get().GetPhysics();
    PxScene* Scene = UPhysicsManager::Get().GetScene();
    PxCooking* Cooking = UPhysicsManager::Get().GetCooking();

    if (!Scene || !Physics || !Cooking)
    {
        return;
    }

    BoneName = Setup->BoneName;
    Owner = InOwner;

    PxVec3 PxLocation = WorldTransform.GetTranslation().ToPxVec3();
    PxQuat PxRotation = WorldTransform.GetRotation().ToPxQuat();

    const FKAggregateGeom& AggGeom = Setup->AggGeom;

    TArray<PxShape*> Shapes;

    FVector CompScale = InOwner->GetComponentScale3D();
    float MaxScale = FMath::Max(FMath::Max(CompScale.X, CompScale.Y), CompScale.Z);
    int Orientation;
    if (MaxScale == CompScale.X) Orientation = 0;
    else if (MaxScale == CompScale.Y) Orientation = 1;
    else Orientation = 2;

    for (const FKSphereElem& Sphere : AggGeom.SphereElems)
    {
        PxSphereGeometry SphereGeom(Sphere.Radius * MaxScale);
        PxVec3 SphereCenter = Sphere.Center.ToPxVec3();
        PxTransform LocalPose(SphereCenter);

        PxShape* Shape = Physics->createShape(SphereGeom, *(Setup->PhysMaterial->GetMaterial()));
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    for (const FKBoxElem& Box : AggGeom.BoxElems)
    {
        PxBoxGeometry BoxGeom((Box.Extent * CompScale).ToPxVec3());
        PxVec3 BoxCenter = Box.Center.ToPxVec3();
        FQuat BoxQuat(FRotator(Box.Rotation));
        PxQuat BoxRotation = BoxQuat.ToPxQuat();
        PxTransform LocalPose(BoxCenter, BoxRotation);

        PxShape* Shape = Physics->createShape(BoxGeom, *(Setup->PhysMaterial->GetMaterial()));
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    for (const FKSphylElem& Sphyl : AggGeom.SphylElems)
    {
        FVector Scale = FVector(CompScale.Z, CompScale.Y, CompScale.X);
        PxCapsuleGeometry CapsuleGeom(Sphyl.Radius * MaxScale, Sphyl.GetScaledHalfLength(CompScale));
        PxVec3 CapsuleCenter = Sphyl.Center.ToPxVec3();
        FQuat CapsuleQuat(FRotator(Sphyl.Rotation));
        PxQuat CapsuleRotation = CapsuleQuat.ToPxQuat();
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

        PxVec3 ConvexCenter = Convex.Center.ToPxVec3();
        FQuat ConvexQuat(FRotator(Convex.Rotation));
        PxQuat ConvexRotation = ConvexQuat.ToPxQuat();
        PxTransform LocalPose(ConvexCenter, ConvexRotation);

        PxShape* Shape = Physics->createShape(ConvexGeom, *(Setup->PhysMaterial->GetMaterial()));
        Shape->setLocalPose(LocalPose);
        Shapes.Add(Shape);
    }

    Actor = UPhysicsManager::Get().SpawnGameObject(this, PxLocation, PxRotation, Shapes, bIsStatic);

    if (UStaticMeshComponent* Comp = Cast<UStaticMeshComponent>(InOwner))
    {
        Comp->SetPhysBody(Actor);
        Actor->rigidBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !Comp->IsUseGravity());
        //Delegate 구독
        UPhysicsManager::Get().OnPhysicsContact.AddUObject(Comp, &UStaticMeshComponent::HandlePhysicsContact);
    }
    if (USkeletalMeshComponent* Comp = Cast<USkeletalMeshComponent>(InOwner))
    {
        Actor->rigidBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
        //스켈레탈 메시 처리
    }
} 

void FBodyInstance::TermBody()
{
    if (Actor)
    {
        UPhysicsManager::Get().RemoveGameObject(Actor);
        if (UStaticMeshComponent* Comp = Cast<UStaticMeshComponent>(reinterpret_cast<FBodyInstance*>(Actor->rigidBody->userData)->Owner))
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

    PxVec3 PxLocation = NewTransform.GetTranslation().ToPxVec3();
    PxQuat PxRotation = NewTransform.GetRotation().ToPxQuat();

    Actor->rigidBody->setGlobalPose(PxTransform(PxLocation, PxRotation));
}
