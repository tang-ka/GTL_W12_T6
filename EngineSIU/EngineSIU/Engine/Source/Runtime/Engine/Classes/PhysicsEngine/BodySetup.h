#pragma once

#include "Physics/BodyInstance.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/BodySetupCore.h"

class UPhysicalMaterial;
class UPrimitiveComponent;

/**
 * BodySetup contains all collision information that is associated with a single asset.
 * A single BodySetup instance is shared among many BodyInstances so that geometry data is not duplicated.
 */
class UBodySetup : public UBodySetupCore
{
    DECLARE_CLASS(UBodySetup, UBodySetupCore)

public:
    UBodySetup();
    ~UBodySetup();

    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;
    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;

    virtual void DisplayProperty() override;

    /** Simplified collision representation of this */
    UPROPERTY_WITH_FLAGS(EditAnywhere, FKAggregateGeom, AggGeom)
    //struct FKAggregateGeom AggGeom;

    /** Should this BodySetup be considered for the bounding box */
    //UPROPERTY(EditAnywhere, Category = BodySetup)
    //uint8 bConsiderForBounds : 1;

    /** If true, the physics triangle mesh will use double sided faces when doing scene queries */
    //UPROPERTY(EditAnywhere, Category = Physics)
    //uint8 bDoubleSidedGeometry : 1;

    /** Should we generate data necessary to support collision on normal versions of this body */
    //UPROPERTY()
    //uint8 bGenerateNonMirroredCollision : 1;

    /** Should we generate data necessary to support collision on mirrored versions of this mesh */
    //UPROPERTY()
    //uint8 bGenerateMirroredCollision : 1;

    /** Flag used to know if we have created the physics meshes from the cooked data yet */
    //uint8 bCreatedPhysicsMeshes : 1;

    /** Indicates that we will never use convex or trimesh shapes */
    //UPROPERTY(EditAnywhere, Category = Collision)
    //uint8 bNeverNeedsCookedCollisionData : 1;

    /** Physical material to use for simple collision on this body */
    //UPROPERTY(EditAnywhere, Category = Physics)
    UPhysicalMaterial* PhysMaterial;

    /** Default properties of the body instance, copied into objects on instantiation */
    //UPROPERTY(EditAnywhere, Category = Collision)
    FBodyInstance DefaultInstance;

    /** Build scale for this body setup */
    //UPROPERTY()
    FVector BuildScale3D;

public:
    // 메서드는 필요한 것만 들고오기
};
