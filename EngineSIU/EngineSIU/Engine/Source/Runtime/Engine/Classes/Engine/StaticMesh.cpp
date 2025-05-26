#include "StaticMesh.h"
#include "Engine/FObjLoader.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"

#include "Engine/Asset/StaticMeshAsset.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BoxElem.h"
#include "Engine/PhysicsManager.h"
#include "Physics/PhysicalMaterial.h"

UObject* UStaticMesh::Duplicate(UObject* InOuter)
{
    // TODO: Context->CopyResource를 사용해서 Buffer복사
    // ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate());
    return nullptr;
}

uint32 UStaticMesh::GetMaterialIndex(FName MaterialSlotName) const
{
    for (uint32 MaterialIndex = 0; MaterialIndex < Materials.Num(); MaterialIndex++)
    {
        if (Materials[MaterialIndex]->MaterialSlotName == MaterialSlotName)
        {
            return MaterialIndex;
        }
    }

    return -1;
}

void UStaticMesh::GetUsedMaterials(TArray<UMaterial*>& OutMaterial) const
{
    for (const FStaticMaterial* Material : Materials)
    {
        OutMaterial.Emplace(Material->Material);
    }
}

FWString UStaticMesh::GetOjbectName() const
{
    return RenderData->ObjectName;
}

void UStaticMesh::SetData(FStaticMeshRenderData* InRenderData)
{
    RenderData = InRenderData;

    if (BodySetup)
        delete BodySetup;

    for (int MaterialIndex = 0; MaterialIndex < RenderData->Materials.Num(); MaterialIndex++)
    {
        FStaticMaterial* NewMaterialSlot = new FStaticMaterial();
        UMaterial* NewMaterial = FObjManager::CreateMaterial(RenderData->Materials[MaterialIndex]);

        NewMaterialSlot->Material = NewMaterial;
        NewMaterialSlot->MaterialSlotName = RenderData->Materials[MaterialIndex].MaterialName;

        Materials.Add(NewMaterialSlot);
    }

    BodySetup = new UBodySetup();

    FKBoxElem Box;
    Box.Center = (RenderData->BoundingBoxMax + RenderData->BoundingBoxMin) * 0.5f;
    Box.Extent = (RenderData->BoundingBoxMax - RenderData->BoundingBoxMin) * 0.5f;
    BodySetup->BoneName = FName("Root");
    BodySetup->AggGeom.BoxElems.Add(Box);
}

void UStaticMesh::SerializeAsset(FArchive& Ar)
{
    if (Ar.IsLoading())
    {
        if (!RenderData)
        {
            RenderData = new FStaticMeshRenderData();
        }
    }

    RenderData->Serialize(Ar);
}

void UStaticMesh::SetPhysMaterial(float InStaticFric, float InDynamicFric, float InRestitution)
{
    BodySetup->PhysMaterial->SetInfo(InStaticFric, InDynamicFric, InRestitution);
}

float UStaticMesh::GetStaticFriction()
{
    return BodySetup->PhysMaterial->GetStaticFriction();
}

float UStaticMesh::GetDynamicFriction()
{
    return BodySetup->PhysMaterial->GetDynamicFriction();
}

float UStaticMesh::GetRestitution()
{
    return BodySetup->PhysMaterial->GetRestitution();
}

UPhysicalMaterial* UStaticMesh::GetPhysMaterial()
{
    return BodySetup->PhysMaterial;
}
