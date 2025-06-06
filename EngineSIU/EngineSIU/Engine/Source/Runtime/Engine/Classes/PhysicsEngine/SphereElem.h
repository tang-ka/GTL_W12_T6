#pragma once

#include "UObject/ObjectMacros.h"
#include "PhysicsEngine/ShapeElem.h"

/** Sphere shape used for collision */
//USTRUCT()
struct FKSphereElem : public FKShapeElem
{
    DECLARE_STRUCT_WITH_SUPER(FKSphereElem, FKShapeElem)

    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, Center)
    
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, Radius)

    FKSphereElem()
        : FKShapeElem(EAggCollisionShape::Sphere)
        , Center(FVector::ZeroVector)
        , Radius(1)
    {

    }

    FKSphereElem(float r)
        : FKShapeElem(EAggCollisionShape::Sphere)
        , Center(FVector::ZeroVector)
        , Radius(r)
    {

    }

    FKSphereElem(FVector c, float r)
        : FKShapeElem(EAggCollisionShape::Sphere)
        , Center(c)
        , Radius(r)
    {

    }

    virtual ~FKSphereElem() = default;

    friend bool operator==(const FKSphereElem& LHS, const FKSphereElem& RHS)
    {
        return (LHS.Center == RHS.Center &&
            LHS.Radius == RHS.Radius);
    }

    // Utility function that builds an FTransform from the current data
    FTransform GetTransform() const
    {
        return FTransform(Center);
    };

    void SetTransform(const FTransform& InTransform)
    {
        Center = InTransform.GetTranslation();
    }

    FORCEINLINE FVector::FReal GetScaledVolume(const FVector& Scale) const { return 1.3333f * PI * FMath::Pow(Radius * Scale.GetAbsMin(), 3); }

    //void GetElemSolid(const FTransform& ElemTM, const FVector& Scale3D, const FMaterialRenderProxy* MaterialRenderProxy, int32 ViewIndex, class FMeshElementCollector& Collector) const;
    //FBoundingBox CalcAABB(const FTransform& BoneTM, float Scale) const;

    void ScaleElem(FVector DeltaSize, float MinSize);

    static EAggCollisionShape::Type StaticShapeType;

    FKSphereElem GetFinalScaled(const FVector& Scale3D, const FTransform& RelativeTM) const;

    /**
     * Finds the shortest distance between the element and a world position. Input and output are given in world space
     * @param	WorldPosition	The point we are trying to get close to
     * @param	BodyToWorldTM	The transform to convert BodySetup into world space
     * @return					The distance between WorldPosition and the shape. 0 indicates WorldPosition is inside the shape.
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
