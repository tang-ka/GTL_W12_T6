#pragma once
#include "Components/MeshComponent.h"
#include "Engine/StaticMesh.h"

#include "Engine/Asset/StaticMeshAsset.h"

class FBodyInstance;
class GameObject;

enum EShape
{
    EBox,
    ESphere,
    ECapsule,
    EConvex
};

class UStaticMeshComponent : public UMeshComponent
{
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)

public:
    UStaticMeshComponent();

    ~UStaticMeshComponent();

    virtual void BeginPlay() override;

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;

    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;

    void SetselectedSubMeshIndex(const int& Value) { SelectedSubMeshIndex = Value; }
    int GetselectedSubMeshIndex() const { return SelectedSubMeshIndex; };

    virtual uint32 GetNumMaterials() const override;
    virtual UMaterial* GetMaterial(uint32 ElementIndex) const override;
    virtual uint32 GetMaterialIndex(FName MaterialSlotName) const override;
    virtual TArray<FName> GetMaterialSlotNames() const override;
    virtual void GetUsedMaterials(TArray<UMaterial*>& Out) const override;

    virtual int CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const override;
    
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }
    void SetStaticMesh(UStaticMesh* Value);

    virtual void SimulatePhysics(bool Value) override;
    virtual void SetPhysMaterial(float InStaticFric, float InDynamicFric, float InRestitution) override;
    virtual void SimulateGravity(bool Value) override;
    virtual void SetIsStatic(bool Value) override;

    void CheckPhysSize();

    void HandlePhysicsContact(USceneComponent* A, USceneComponent* B);

    void HandleContactPoint(FVector Pos, FVector Norm);

    void SetBodySetupGeom(bool Box, bool Sphere, bool Capsule, bool Convex);

    void GetBodySetupGeom(bool& OutBox, bool& OutSphere, bool& OutCapsule, bool& OutConvex);

protected:
    UStaticMesh* StaticMesh = nullptr;
    int SelectedSubMeshIndex = -1;
    bool bIsBox = true;
    bool bIsSphere = false;
    bool bIsCapsule = false;
    bool bIsConvex = false;
    EShape CurShape = EShape::EBox;
};
