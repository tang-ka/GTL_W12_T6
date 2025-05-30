#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Components/Material/Material.h"
#include "Define.h"

class UBodySetup;
class UPhysicalMaterial;

struct FStaticMeshRenderData;

class UStaticMesh : public UObject
{
    DECLARE_CLASS(UStaticMesh, UObject)

public:
    UStaticMesh() = default;

    virtual UObject* Duplicate(UObject* InOuter) override;

    const TArray<FStaticMaterial*>& GetMaterials() const { return Materials; }
    uint32 GetMaterialIndex(FName MaterialSlotName) const;
    void GetUsedMaterials(TArray<UMaterial*>& OutMaterial) const;
    FStaticMeshRenderData* GetRenderData() const { return RenderData; }

    //ObjectName은 경로까지 포함
    FWString GetOjbectName() const;

    void SetData(FStaticMeshRenderData* InRenderData);

    virtual void SerializeAsset(FArchive& Ar) override;

    UBodySetup* GetBodySetup() { return BodySetup; }

    void SetPhysMaterial(float InStaticFric, float InDynamicFric, float InRestitution);

    float GetStaticFriction();
    float GetDynamicFriction();
    float GetRestitution();

    UPhysicalMaterial* GetPhysMaterial();

private:
    FStaticMeshRenderData* RenderData = nullptr;
    TArray<FStaticMaterial*> Materials;

    UBodySetup* BodySetup = nullptr; // 물리 엔진에서 사용되는 바디 설정
};
