#include "SkeletalMeshRenderPassBase.h"

#include "Animation/Skeleton.h"
#include "UObject/UObjectIterator.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/EditorEngine.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/Casts.h"
#include "Editor/PropertyEditor/ShowFlags.h"
#include "Engine/Asset/SkeletalMeshAsset.h"
#include "Engine/AssetManager.h"
#include "RendererHelpers.h"

class UEditorEngine;

void FSkeletalMeshRenderPassBase::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManager);
}

void FSkeletalMeshRenderPassBase::InitializeShadowManager(class FShadowManager* InShadowManager)
{
    ShadowManager = InShadowManager;
}

void FSkeletalMeshRenderPassBase::PrepareRenderArr()
{
    for (const auto Iter : TObjectRange<USkeletalMeshComponent>())
    {
        if (Iter->GetWorld() != GEngine->ActiveWorld)
        {
            continue;
        }
        SkeletalMeshComponents.Add(Iter);
    }
}

void FSkeletalMeshRenderPassBase::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);

    Render_Internal(Viewport);

    CleanUpRender(Viewport);
}

void FSkeletalMeshRenderPassBase::ClearRenderArr()
{
    SkeletalMeshComponents.Empty();
}

void FSkeletalMeshRenderPassBase::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FSkeletalMeshRenderPassBase::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FSkeletalMeshRenderPassBase::Render_Internal(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    RenderAllSkeletalMeshes(Viewport);
}

void FSkeletalMeshRenderPassBase::RenderAllSkeletalMeshes(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    for (const USkeletalMeshComponent* Comp : SkeletalMeshComponents)
    {
        if (!Comp || !Comp->GetSkeletalMeshAsset())
        {
            continue;
        }
        const FSkeletalMeshRenderData* RenderData = Comp->GetCPUSkinning() ? Comp->GetCPURenderData() : Comp->GetSkeletalMeshAsset()->GetRenderData();
        if (RenderData == nullptr)
        {
            continue;
        }

        const UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);

        FMatrix WorldMatrix = Comp->GetWorldMatrix();
        FVector4 UUIDColor = Comp->EncodeUUID() / 255.0f;
        const bool bIsSelected = (Engine && Engine->GetSelectedActor() == Comp->GetOwner());

        UpdateObjectConstant(WorldMatrix, UUIDColor, bIsSelected);

        RenderSkeletalMesh(RenderData);

        if (Viewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            FEngineLoop::PrimitiveDrawBatch.AddAABBToBatch(Comp->GetBoundingBox(), Comp->GetComponentLocation(), WorldMatrix);
        }
    }
}

void FSkeletalMeshRenderPassBase::RenderSkeletalMesh(const FSkeletalMeshRenderData* RenderData) const
{
    constexpr UINT Stride = sizeof(FSkeletalMeshVertex);
    constexpr UINT Offset = 0;

    FCPUSkinningConstants CPUSkinningData;
    CPUSkinningData.bCPUSKinning = USkeletalMeshComponent::GetCPUSkinning();
    BufferManager->UpdateConstantBuffer(TEXT("FCPUSkinningConstants"), CPUSkinningData);
    
    FVertexInfo VertexInfo;
    if (CPUSkinningData.bCPUSKinning)
    {
        BufferManager->CreateDynamicVertexBuffer(RenderData->ObjectName, RenderData->Vertices, VertexInfo);
        BufferManager->UpdateDynamicVertexBuffer(RenderData->ObjectName, RenderData->Vertices);
    }
    else
    {
        BufferManager->CreateVertexBuffer(RenderData->ObjectName, RenderData->Vertices, VertexInfo);
    }
    
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &Stride, &Offset);
    
    FIndexInfo IndexInfo;
    BufferManager->CreateIndexBuffer(RenderData->ObjectName, RenderData->Indices, IndexInfo);
    if (IndexInfo.IndexBuffer)
    {
        Graphics->DeviceContext->IASetIndexBuffer(IndexInfo.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    }
    else
    {
        Graphics->DeviceContext->Draw(RenderData->Vertices.Num(), 0);
        return;
    }

    for (int SubMeshIndex = 0; SubMeshIndex < RenderData->MaterialSubsets.Num(); SubMeshIndex++)
    {
        FName MaterialName = RenderData->MaterialSubsets[SubMeshIndex].MaterialName;
        UMaterial* Material = UAssetManager::Get().GetMaterial(MaterialName);
        FMaterialInfo MaterialInfo = Material->GetMaterialInfo();
        MaterialUtils::UpdateMaterial(BufferManager, Graphics, MaterialInfo);

        const uint32 StartIndex = RenderData->MaterialSubsets[SubMeshIndex].IndexStart;
        const uint32 IndexCount = RenderData->MaterialSubsets[SubMeshIndex].IndexCount; 
        Graphics->DeviceContext->DrawIndexed(IndexCount, StartIndex, 0);
    }
}

void FSkeletalMeshRenderPassBase::RenderSkeletalMesh(ID3D11Buffer* Buffer, UINT VerticesNum) const
{
}

void FSkeletalMeshRenderPassBase::RenderSkeletalMesh(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, UINT IndicesNum) const
{
}
