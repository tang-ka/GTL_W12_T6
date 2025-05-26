#pragma once

#include "UObject/ObjectMacros.h"
#include "PhysicsEngine/ShapeElem.h"

class FMeshElementCollector;

/** Capsule shape used for collision. Z axis is capsule axis. */
//USTRUCT()
struct FKSphylElem : public FKShapeElem
{
    DECLARE_STRUCT_WITH_SUPER(FKSphylElem, FKShapeElem)

    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, Center)

    UPROPERTY_WITH_FLAGS(EditAnywhere, FRotator, Rotation)

    UPROPERTY_WITH_FLAGS(EditAnywhere, float, Radius)

    UPROPERTY_WITH_FLAGS(EditAnywhere, float, Length)

    FKSphylElem()
        : FKShapeElem(EAggCollisionShape::Sphyl)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Radius(1), Length(1)
    {

    }

    FKSphylElem(float InRadius, float InLength)
        : FKShapeElem(EAggCollisionShape::Sphyl)
        , Center(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Radius(InRadius), Length(InLength)
    {

    }

    virtual ~FKSphylElem() = default;

    friend bool operator==(const FKSphylElem& LHS, const FKSphylElem& RHS)
    {
        return (LHS.Center == RHS.Center &&
            LHS.Rotation == RHS.Rotation &&
            LHS.Radius == RHS.Radius &&
            LHS.Length == RHS.Length);
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

    FORCEINLINE FVector::FReal GetScaledVolume(const FVector& Scale3D) const
    {
        FVector::FReal ScaledRadius = GetScaledRadius(Scale3D);
        FVector::FReal ScaledLength = GetScaledCylinderLength(Scale3D);
        return PI * FMath::Square(ScaledRadius) * (1.3333f * ScaledRadius + ScaledLength);
    }

    //FBox CalcAABB(const FTransform& BoneTM, float Scale) const;
    void ScaleElem(FVector DeltaSize, float MinSize);

    FKSphylElem GetFinalScaled(const FVector& Scale3D, const FTransform& RelativeTM) const;

    /** Returns the scaled radius for this Sphyl, which is determined by the Max scale on X/Y and clamped by half the total length */
    float GetScaledRadius(const FVector& Scale3D) const;
    /** Returns the scaled length of the cylinder part of the Sphyl **/
    float GetScaledCylinderLength(const FVector& Scale3D) const;
    /** Returns half of the total scaled length of the Sphyl, which includes the scaled top and bottom caps */
    float GetScaledHalfLength(const FVector& Scale3D) const;

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

    static EAggCollisionShape::Type StaticShapeType;
};
