#pragma once

#include "UObject/ObjectMacros.h"
#include "ShapeElem.h"

struct FKAggregateGeom
{
    // 각 Geometry Shape에 대한 내용
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "Spheres", TitleProperty = "Name"))
    TArray<FKSphereElem> SphereElems;
    //
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "Boxes", TitleProperty = "Name"))
    TArray<FKBoxElem> BoxElems;
    //
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "Capsules", TitleProperty = "Name"))
    TArray<FKSphylElem> SphylElems;
    //
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "Convex Elements", TitleProperty = "Name"))
    TArray<FKConvexElem> ConvexElems;
    //
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "Tapered Capsules", TitleProperty = "Name"))
    //TArray<FKTaperedCapsuleElem> TaperedCapsuleElems;
    //
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "Level Sets", TitleProperty = "Name"))
    //TArray<FKLevelSetElem> LevelSetElems;
    //
    //UPROPERTY(EditAnywhere, editfixedsize, Category = "Aggregate Geometry", meta = (DisplayName = "(Experimental) Skinned Level Sets", TitleProperty = "Name"), Experimental)
    //TArray<FKSkinnedLevelSetElem> SkinnedLevelSetElems;

    FKAggregateGeom()
        : RenderInfoPtr(nullptr)
    {
    }

    FKAggregateGeom(const FKAggregateGeom& Other)
        : RenderInfoPtr(nullptr)
    {
        CloneAgg(Other);
    }

    const FKAggregateGeom& operator=(const FKAggregateGeom& Other)
    {
        FreeRenderInfo();
        CloneAgg(Other);
        return *this;
    }

    // 모든 형상의 개수 합쳐서 던져주기
    int32 GetElementCount() const
    {
        return SphereElems.Num() + SphylElems.Num() + BoxElems.Num() + ConvexElems.Num() /*+ TaperedCapsuleElems.Num() + LevelSetElems.Num() + SkinnedLevelSetElems.Num()*/;
        //return SphereElems.Num() + SphylElems.Num() + BoxElems.Num() + ConvexElems.Num() + TaperedCapsuleElems.Num() + LevelSetElems.Num() + SkinnedLevelSetElems.Num();
    }

    int32 GetElementCount(EAggCollisionShape::Type Type) const
    {
        switch (Type)
        {
        case EAggCollisionShape::Sphere:
            return SphereElems.Num();
            break;
        case EAggCollisionShape::Box:
            return BoxElems.Num();
            break;
        case EAggCollisionShape::Sphyl:
            return SphylElems.Num();
            break;
        case EAggCollisionShape::Convex:
            return ConvexElems.Num();
            break;
        default:
            return -1;
            break;
        }
    }

    FKShapeElem* GetElement(const EAggCollisionShape::Type Type, const int32 Index)
    {

        switch (Type)
        {
        case EAggCollisionShape::Sphere:
            if (SphereElems.IsValidIndex(Index)) { return &SphereElems[Index]; }
            break;
        case EAggCollisionShape::Box:
            if (BoxElems.IsValidIndex(Index)) { return &BoxElems[Index]; }
            break;
        case EAggCollisionShape::Sphyl:
            if (SphylElems.IsValidIndex(Index)) { return &SphylElems[Index]; }
            break;
        case EAggCollisionShape::Convex:
            if (ConvexElems.IsValidIndex(Index)) { return &ConvexElems[Index]; }
            break;
        default:
            return nullptr;
            break;
        }
    }

    FKShapeElem* GetElement(const int32 InIndex)
    {
        /*int Index = InIndex;
        if (Index < SphereElems.Num()) { return &SphereElems[Index]; }
        Index -= SphereElems.Num();
        if (Index < BoxElems.Num()) { return &BoxElems[Index]; }
        Index -= BoxElems.Num();
        if (Index < SphylElems.Num()) { return &SphylElems[Index]; }
        Index -= SphylElems.Num();
        if (Index < ConvexElems.Num()) { return &ConvexElems[Index]; }
        Index -= ConvexElems.Num();
        if (Index < TaperedCapsuleElems.Num()) { return &TaperedCapsuleElems[Index]; }
        Index -= TaperedCapsuleElems.Num();
        if (Index < LevelSetElems.Num()) { return &LevelSetElems[Index]; }
        Index -= LevelSetElems.Num();
        if (Index < SkinnedLevelSetElems.Num()) { return &SkinnedLevelSetElems[Index]; }
        ensure(false);*/
        return nullptr;
    }

    const FKShapeElem* GetElement(const int32 InIndex) const
    {
        /*int Index = InIndex;
        if (Index < SphereElems.Num()) { return &SphereElems[Index]; }
        Index -= SphereElems.Num();
        if (Index < BoxElems.Num()) { return &BoxElems[Index]; }
        Index -= BoxElems.Num();
        if (Index < SphylElems.Num()) { return &SphylElems[Index]; }
        Index -= SphylElems.Num();
        if (Index < ConvexElems.Num()) { return &ConvexElems[Index]; }
        Index -= ConvexElems.Num();
        if (Index < TaperedCapsuleElems.Num()) { return &TaperedCapsuleElems[Index]; }
        Index -= TaperedCapsuleElems.Num();
        if (Index < LevelSetElems.Num()) { return &LevelSetElems[Index]; }
        Index -= LevelSetElems.Num();
        if (Index < SkinnedLevelSetElems.Num()) { return &SkinnedLevelSetElems[Index]; }
        ensure(false);*/
        return nullptr;
    }

    const FKShapeElem* GetElementByName(const FName InName) const
    {
        /*if (const FKShapeElem* FoundSphereElem = GetElementByName<FKSphereElem>(MakeArrayView(SphereElems), InName))
        {
            return FoundSphereElem;
        }
        else if (const FKShapeElem* FoundBoxElem = GetElementByName<FKBoxElem>(MakeArrayView(BoxElems), InName))
        {
            return FoundBoxElem;
        }
        else if (const FKShapeElem* FoundSphylElem = GetElementByName<FKSphylElem>(MakeArrayView(SphylElems), InName))
        {
            return FoundSphylElem;
        }
        else if (const FKShapeElem* FoundConvexElem = GetElementByName<FKConvexElem>(MakeArrayView(ConvexElems), InName))
        {
            return FoundConvexElem;
        }
        else if (const FKShapeElem* FoundTaperedCapsuleElem = GetElementByName<FKTaperedCapsuleElem>(MakeArrayView(TaperedCapsuleElems), InName))
        {
            return FoundTaperedCapsuleElem;
        }
        else if (const FKShapeElem* FoundLevelSetElem = GetElementByName<FKLevelSetElem>(MakeArrayView(LevelSetElems), InName))
        {
            return FoundLevelSetElem;
        }
        else if (const FKShapeElem* FoundSkinnedLevelSetElem = GetElementByName<FKSkinnedLevelSetElem>(MakeArrayView(SkinnedLevelSetElems), InName))
        {
            return FoundSkinnedLevelSetElem;
        }*/

        return nullptr;
    }

    int32 GetElementIndexByName(const FName InName) const
    {
        /*int32 FoundIndex = GetElementIndexByName<FKSphereElem>(MakeArrayView(SphereElems), InName);
        int32 StartIndex = 0;
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }
        StartIndex += SphereElems.Num();

        FoundIndex = GetElementIndexByName<FKBoxElem>(MakeArrayView(BoxElems), InName);
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }
        StartIndex += BoxElems.Num();

        FoundIndex = GetElementIndexByName<FKSphylElem>(MakeArrayView(SphylElems), InName);
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }
        StartIndex += SphylElems.Num();

        FoundIndex = GetElementIndexByName<FKConvexElem>(MakeArrayView(ConvexElems), InName);
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }
        StartIndex += ConvexElems.Num();

        FoundIndex = GetElementIndexByName<FKTaperedCapsuleElem>(MakeArrayView(TaperedCapsuleElems), InName);
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }
        StartIndex += TaperedCapsuleElems.Num();

        FoundIndex = GetElementIndexByName<FKLevelSetElem>(MakeArrayView(LevelSetElems), InName);
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }
        StartIndex += LevelSetElems.Num();

        FoundIndex = GetElementIndexByName<FKSkinnedLevelSetElem>(MakeArrayView(SkinnedLevelSetElems), InName);
        if (FoundIndex != INDEX_NONE)
        {
            return FoundIndex + StartIndex;
        }*/

        return INDEX_NONE;
    }

    void EmptyElements()
    {
        /*BoxElems.Empty();
        ConvexElems.Empty();
        SphylElems.Empty();
        SphereElems.Empty();
        TaperedCapsuleElems.Empty();
        LevelSetElems.Empty();
        SkinnedLevelSetElems.Empty();*/

        FreeRenderInfo();
    }

    //void GetAggGeom(const FTransform& Transform, const FColor Color, const FMaterialRenderProxy* MatInst, bool bPerHullColor, bool bDrawSolid, bool bOutputVelocity, int32 ViewIndex, class FMeshElementCollector& Collector) const;

    /** Release the RenderInfo (if its there) and safely clean up any resources. Call on the game thread. */
    void FreeRenderInfo();

    /** Returns the volume of this element */
    FVector::FReal GetScaledVolume(const FVector& Scale3D) const;

private:

    /** Helper function for safely copying instances */
    void CloneAgg(const FKAggregateGeom& Other)
    {
       /* SphereElems = Other.SphereElems;
        BoxElems = Other.BoxElems;
        SphylElems = Other.SphylElems;
        ConvexElems = Other.ConvexElems;
        TaperedCapsuleElems = Other.TaperedCapsuleElems;
        LevelSetElems = Other.LevelSetElems;
        SkinnedLevelSetElems = Other.SkinnedLevelSetElems;*/
    }

    mutable std::atomic<class FKConvexGeomRenderInfo*> RenderInfoPtr;
    //mutable UE::FMutex RenderInfoLock;
};
