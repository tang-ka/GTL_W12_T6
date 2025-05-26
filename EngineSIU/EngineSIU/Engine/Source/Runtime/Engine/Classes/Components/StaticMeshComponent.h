#pragma once
#include "Components/MeshComponent.h"
#include "Engine/StaticMesh.h"

#include "Engine/Asset/StaticMeshAsset.h"

class FBodyInstance;
class GameObject;

class UStaticMeshComponent : public UMeshComponent
{
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)

public:
    UStaticMeshComponent();

    ~UStaticMeshComponent();

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

    bool ShouldSimulatePhysics() { return bSimulatePhysics; }
    void SimulatePhysics(bool Value);

    void SetPhysMaterial(float InStaticFric, float InDynamicFric, float InRestitution);

    void SetPhysBody(GameObject* InBody);

    GameObject* GetPhysBody() { return PhysicsBody; }

    bool IsUseGravity() { return bSimulateGravity; }

    void SimulateGravity(bool Value);

    void CheckPhysSize();

    bool GetIsStatic() { return bIsStatic; }

    void SetIsStatic(bool Value);

protected:
    UStaticMesh* StaticMesh = nullptr;
    int SelectedSubMeshIndex = -1;
    FBodyInstance* Body = nullptr;
    bool bSimulatePhysics = false;
    GameObject* PhysicsBody = nullptr;
    bool bSimulateGravity = false;
    bool bIsStatic = false;
};
