#pragma once

#include "UObject/ObjectMacros.h"
#include "PhysicsEngine/ShapeElem.h"

class FMaterialRenderProxy;
class FMeshElementCollector;

/** Box shape used for collision */
//USTRUCT()
struct FKBoxElem : public FKShapeElem
{
    DECLARE_STRUCT_WITH_SUPER(FKBoxElem, FKShapeElem)
    /** Position of the box's origin */
    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, Center)
    //FVector Center;

    /** Rotation of the box */
    //UPROPERTY(Category = Box, EditAnywhere, meta = (ClampMin = "-360", ClampMax = "360"))
    UPROPERTY_WITH_FLAGS(EditAnywhere, FRotator, Rotation)
    //FRotator Rotation;

    /** Extent of the box */
    //UPROPERTY(Category = Box, EditAnywhere, meta = (DisplayName = "X Extent"))
    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, Extent)
    //FVector Extent;


    FKBoxElem()
        : FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Extent(FVector(1.0f, 1.0f, 1.0f))
    {

    }

    FKBoxElem(float s)
        : FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Extent(FVector(s, s, s))
    {

    }

    FKBoxElem(float InX, float InY, float InZ)
        : FKShapeElem(EAggCollisionShape::Box)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Extent(FVector(InX, InY, InZ))
    {

    }

    FKBoxElem(FVector C, FVector E)
        : FKShapeElem(EAggCollisionShape::Box)
        , Center(C)
        , Rotation(FRotator::ZeroRotator)
        , Extent(E)
    {

    }

    virtual ~FKBoxElem() = default;

    friend bool operator==(const FKBoxElem& LHS, const FKBoxElem& RHS)
    {
        return (LHS.Center == RHS.Center &&
            LHS.Rotation == RHS.Rotation &&
            LHS.Extent.X == RHS.Extent.X &&
            LHS.Extent.Y == RHS.Extent.Y &&
            LHS.Extent.Z == RHS.Extent.Z);
    };

    // Utility function that builds an FTransform from the current data
    FTransform GetTransform() const
    {
        return FTransform(Rotation, Center);
    };

    void SetTransform(const FTransform& InTransform)
    {
        Rotation = InTransform.Rotator();
        Center = InTransform.GetTranslation();
    }

    FORCEINLINE FVector::FReal GetScaledVolume(const FVector& Scale3D) const { return FMath::Abs(Scale3D.X * Scale3D.Y * Scale3D.Z * Extent.X * Extent.Y * Extent.Z); }

    //FBox CalcAABB(const FTransform& BoneTM, float Scale) const;

    //void ScaleElem(FVector DeltaSize, float MinSize);

    FKBoxElem GetFinalScaled(const FVector& Scale3D, const FTransform& RelativeTM) const;

    static EAggCollisionShape::Type StaticShapeType;

    /**
     * Finds the shortest distance between the element and a world position. Input and output are given in world space
     * @param	WorldPosition	The point we are trying to get close to
     * @param	BodyToWorldTM	The transform to convert BodySetup into world space
     * @return					The distance between WorldPosition and the shape. 0 indicates WorldPosition is inside one of the shapes.
     */
    float GetShortestDistanceToPoint(const FVector& WorldPosition, const FTransform& BodyToWorldTM) const;


    /**
     * Finds the closest point on the shape given a world position. Input and output are given in world space
     * @param	WorldPosition			The point we are trying to get close to
     * @param	BodyToWorldTM			The transform to convert BodySetup into world space
     * @param	ClosestWorldPosition	The closest point on the shape in world space
     * @param	Normal					The normal of the feature associated with ClosestWorldPosition.
     * @return							The distance between WorldPosition and the shape. 0 indicates WorldPosition is inside the shape.
     */
    float GetClosestPointAndNormal(const FVector& WorldPosition, const FTransform& BodyToWorldTM, FVector& ClosestWorldPosition, FVector& Normal) const;
};
