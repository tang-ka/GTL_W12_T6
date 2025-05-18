#include "StaticMeshRenderPass.h"

#include <array>

#include "EngineLoop.h"
#include "World/World.h"

#include "RendererHelpers.h"
#include "ShadowManager.h"
#include "ShadowRenderPass.h"
#include "UnrealClient.h"
#include "Math/JungleMath.h"

#include "UObject/UObjectIterator.h"
#include "UObject/Casts.h"

#include "D3D11RHI/DXDBufferManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"

#include "Components/StaticMeshComponent.h"

#include "BaseGizmos/GizmoBaseComponent.h"
#include "Engine/EditorEngine.h"

#include "PropertyEditor/ShowFlags.h"

#include "UnrealEd/EditorViewportClient.h"
#include "Components/Light/PointLightComponent.h"
#include "Engine/AssetManager.h"


FStaticMeshRenderPass::FStaticMeshRenderPass()
    : BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
{
}

FStaticMeshRenderPass::~FStaticMeshRenderPass()
{
    ReleaseShader();
}

void FStaticMeshRenderPass::CreateShader()
{
    // Begin Debug Shaders
    HRESULT hr = ShaderManager->AddPixelShader(L"CommonMeshPixelShaderDepth", L"Shaders/CommonMeshPixelShaderDepth.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
    hr = ShaderManager->AddPixelShader(L"CommonMeshPixelShaderWorldNormal", L"Shaders/CommonMeshPixelShaderWorldNormal.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
    hr = ShaderManager->AddPixelShader(L"CommonMeshPixelShaderWorldTangent", L"Shaders/CommonMeshPixelShaderWorldTangent.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
    // End Debug Shaders

#pragma region UberShader
    D3D_SHADER_MACRO DefinesGouraud[] =
    {
        { GOURAUD, "1" },
        { nullptr, nullptr }
    };
    hr = ShaderManager->AddPixelShader(L"GOURAUD_CommonMeshPixelShader", L"Shaders/CommonMeshPixelShader.hlsl", "mainPS", DefinesGouraud);
    if (FAILED(hr))
    {
        return;
    }
    
    D3D_SHADER_MACRO DefinesLambert[] =
    {
        { LAMBERT, "1" },
        { nullptr, nullptr }
    };
    hr = ShaderManager->AddPixelShader(L"LAMBERT_CommonMeshPixelShader", L"Shaders/CommonMeshPixelShader.hlsl", "mainPS", DefinesLambert);
    if (FAILED(hr))
    {
        return;
    }
    
    D3D_SHADER_MACRO DefinesBlinnPhong[] =
    {
        { PHONG, "1" },
        { nullptr, nullptr }
    };
    hr = ShaderManager->AddPixelShader(L"PHONG_CommonMeshPixelShader", L"Shaders/CommonMeshPixelShader.hlsl", "mainPS", DefinesBlinnPhong);
    if (FAILED(hr))
    {
        return;
    }
    
    D3D_SHADER_MACRO DefinesPBR[] =
    {
        { PBR, "1" },
        { nullptr, nullptr }
    };
    hr = ShaderManager->AddPixelShader(L"PBR_CommonMeshPixelShader", L"Shaders/CommonMeshPixelShader.hlsl", "mainPS", DefinesPBR);
    if (FAILED(hr))
    {
        return;
    }
#pragma endregion UberShader
}

void FStaticMeshRenderPass::ReleaseShader()
{
    
}

void FStaticMeshRenderPass::ChangeViewMode(EViewModeIndex ViewMode)
{
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = ShaderManager->GetInputLayoutByKey(L"StaticMeshVertexShader");

    if (ViewMode == EViewModeIndex::VMI_Lit_Gouraud)
    {
        VertexShader = ShaderManager->GetVertexShaderByKey(L"GOURAUD_StaticMeshVertexShader");
    }
    else
    {
        VertexShader = ShaderManager->GetVertexShaderByKey(L"StaticMeshVertexShader");
    }
    
    switch (ViewMode)
    {
    case EViewModeIndex::VMI_Lit_Gouraud:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"GOURAUD_CommonMeshPixelShader");
        break;
    case EViewModeIndex::VMI_Lit_BlinnPhong:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"PHONG_CommonMeshPixelShader");
        break;
    case EViewModeIndex::VMI_LIT_PBR:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"PBR_CommonMeshPixelShader");
        break;
    case EViewModeIndex::VMI_SceneDepth:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"CommonMeshPixelShaderDepth");
        break;
    case EViewModeIndex::VMI_WorldNormal:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"CommonMeshPixelShaderWorldNormal");
        break;
    case EViewModeIndex::VMI_WorldTangent:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"CommonMeshPixelShaderWorldTangent");
        break;
    default:
        PixelShader = ShaderManager->GetPixelShaderByKey(L"LAMBERT_CommonMeshPixelShader");
        break;
    }

    UpdateLitUnlitConstant(ViewMode < EViewModeIndex::VMI_Unlit);

    // Rasterizer
    Graphics->ChangeRasterizer(ViewMode);

    // Setup
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(InputLayout);
}

void FStaticMeshRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManager;
    
    CreateShader();
}

void FStaticMeshRenderPass::InitializeShadowManager(class FShadowManager* InShadowManager)
{
    ShadowManager = InShadowManager;
}

void FStaticMeshRenderPass::PrepareRenderArr()
{
    for (const auto Iter : TObjectRange<UStaticMeshComponent>())
    {
        if (!Cast<UGizmoBaseComponent>(Iter) && Iter->GetWorld() == GEngine->ActiveWorld)
        {
            if (Iter->GetOwner() && !Iter->GetOwner()->IsHidden())
            {
                StaticMeshComponents.Add(Iter);
            }
        }
    }
}

void FStaticMeshRenderPass::PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport) 
{
    const EViewModeIndex ViewMode = Viewport->GetViewMode();

    ChangeViewMode(ViewMode);

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    TArray<FString> PSBufferKeys = {
        TEXT("FLightInfoBuffer"),
        TEXT("FMaterialConstants"),
        TEXT("FLitUnlitConstants"),
        TEXT("FSubMeshConstants"),
        TEXT("FTextureConstants"),
        TEXT("FIsShadowConstants"),
    };

    BufferManager->BindConstantBuffers(PSBufferKeys, 0, EShaderStage::Pixel);

    BufferManager->BindConstantBuffer(TEXT("FLightInfoBuffer"), 0, EShaderStage::Vertex);
    BufferManager->BindConstantBuffer(TEXT("FMaterialConstants"), 1, EShaderStage::Vertex);
    BufferManager->BindConstantBuffer(TEXT("FObjectConstantBuffer"), 12, EShaderStage::Vertex);
    
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    const EResourceType ResourceType = EResourceType::ERT_Scene;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(ResourceType);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    Graphics->DeviceContext->OMSetBlendState(Graphics->BlendState_AlphaBlend, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetBlendState(Graphics->BlendState_PremultipliedAlpha, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_DepthWriteDisabled, 0);
    //Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    //Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_Default, 0);
}

void FStaticMeshRenderPass::UpdateObjectConstant(const FMatrix& WorldMatrix, const FVector4& UUIDColor, bool bIsSelected) const
{
    FObjectConstantBuffer ObjectData = {};
    ObjectData.WorldMatrix = WorldMatrix;
    ObjectData.InverseTransposedWorld = FMatrix::Transpose(FMatrix::Inverse(WorldMatrix));
    ObjectData.UUIDColor = UUIDColor;
    ObjectData.bIsSelected = bIsSelected;
    
    BufferManager->UpdateConstantBuffer(TEXT("FObjectConstantBuffer"), ObjectData);
}

void FStaticMeshRenderPass::UpdateLitUnlitConstant(int32 IsLit) const
{
    FLitUnlitConstants Data;
    Data.bIsLit = IsLit;
    BufferManager->UpdateConstantBuffer(TEXT("FLitUnlitConstants"), Data);
}

void FStaticMeshRenderPass::RenderPrimitive(FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const
{
    UINT Stride = sizeof(FStaticMeshVertex);
    UINT Offset = 0;

    FVertexInfo VertexInfo;
    BufferManager->CreateVertexBuffer(RenderData->ObjectName, RenderData->Vertices, VertexInfo);

    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &Stride, &Offset);

    FIndexInfo IndexInfo;
    BufferManager->CreateIndexBuffer(RenderData->ObjectName, RenderData->Indices, IndexInfo);
    if (IndexInfo.IndexBuffer)
    {
        Graphics->DeviceContext->IASetIndexBuffer(IndexInfo.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    }

    if (RenderData->MaterialSubsets.Num() == 0)
    {
        Graphics->DeviceContext->DrawIndexed(RenderData->Indices.Num(), 0, 0);
        return;
    }

    for (int SubMeshIndex = 0; SubMeshIndex < RenderData->MaterialSubsets.Num(); SubMeshIndex++)
    {
        uint32 MaterialIndex = RenderData->MaterialSubsets[SubMeshIndex].MaterialIndex;

        FSubMeshConstants SubMeshData = (SubMeshIndex == SelectedSubMeshIndex) ? FSubMeshConstants(true) : FSubMeshConstants(false);

        BufferManager->UpdateConstantBuffer(TEXT("FSubMeshConstants"), SubMeshData);

        if (!OverrideMaterials.IsEmpty() && OverrideMaterials.Num() >= MaterialIndex && OverrideMaterials[MaterialIndex] != nullptr)
        {
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, OverrideMaterials[MaterialIndex]->GetMaterialInfo());
        }
        else if (!Materials.IsEmpty() && Materials.Num() >= MaterialIndex && Materials[MaterialIndex] != nullptr)
        {
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, Materials[MaterialIndex]->Material->GetMaterialInfo());
        }
        else if (UMaterial* Mat = UAssetManager::Get().GetMaterial(RenderData->MaterialSubsets[SubMeshIndex].MaterialName))
        {
            MaterialUtils::UpdateMaterial(BufferManager, Graphics, Mat->GetMaterialInfo());
        }

        uint32 StartIndex = RenderData->MaterialSubsets[SubMeshIndex].IndexStart;
        uint32 IndexCount = RenderData->MaterialSubsets[SubMeshIndex].IndexCount;
        Graphics->DeviceContext->DrawIndexed(IndexCount, StartIndex, 0);
    }
}

void FStaticMeshRenderPass::RenderPrimitive(ID3D11Buffer* pBuffer, UINT NumVertices) const
{
    UINT Stride = sizeof(FStaticMeshVertex);
    UINT Offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &Offset);
    Graphics->DeviceContext->Draw(NumVertices, 0);
}

void FStaticMeshRenderPass::RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT NumVertices, ID3D11Buffer* pIndexBuffer, UINT NumIndices) const
{
    UINT Stride = sizeof(FStaticMeshVertex);
    UINT Offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &Stride, &Offset);
    Graphics->DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Graphics->DeviceContext->DrawIndexed(NumIndices, 0, 0);
}

void FStaticMeshRenderPass::RenderAllStaticMeshes(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    for (UStaticMeshComponent* Comp : StaticMeshComponents)
    {
        if (!Comp || !Comp->GetStaticMesh())
        {
            continue;
        }

        FStaticMeshRenderData* RenderData = Comp->GetStaticMesh()->GetRenderData();
        if (RenderData == nullptr)
        {
            continue;
        }

        UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);

        USceneComponent* SelectedComponent = Engine->GetSelectedComponent();
        AActor* SelectedActor = Engine->GetSelectedActor();

        USceneComponent* TargetComponent = nullptr;

        if (SelectedComponent != nullptr)
        {
            TargetComponent = SelectedComponent;
        }
        else if (SelectedActor != nullptr)
        {
            TargetComponent = SelectedActor->GetRootComponent();
        }

        FMatrix WorldMatrix = Comp->GetWorldMatrix();
        FVector4 UUIDColor = Comp->EncodeUUID() / 255.0f;
        const bool bIsSelected = (Engine && TargetComponent == Comp);

        UpdateObjectConstant(WorldMatrix, UUIDColor, bIsSelected);

        RenderPrimitive(RenderData, Comp->GetStaticMesh()->GetMaterials(), Comp->GetOverrideMaterials(), Comp->GetselectedSubMeshIndex());

        if (Viewport->GetShowFlag() & static_cast<uint64>(EEngineShowFlags::SF_AABB))
        {
            FEngineLoop::PrimitiveDrawBatch.AddAABBToBatch(Comp->GetBoundingBox(), Comp->GetComponentLocation(), WorldMatrix);
        }
    }
}

void FStaticMeshRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    ShadowManager->BindResourcesForSampling();

    PrepareRenderState(Viewport);

    RenderAllStaticMeshes(Viewport);

    // 렌더 타겟 해제
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
    Graphics->DeviceContext->PSSetShaderResources(static_cast<int>(EShaderSRVSlot::SRV_PointLight), 1, NullSRV); // t51 슬롯을 NULL로 설정
    Graphics->DeviceContext->PSSetShaderResources(static_cast<int>(EShaderSRVSlot::SRV_DirectionalLight), 1, NullSRV); // t51 슬롯을 NULL로 설정
    Graphics->DeviceContext->PSSetShaderResources(static_cast<int>(EShaderSRVSlot::SRV_SpotLight), 1, NullSRV); // t51 슬롯을 NULL로 설정

    // 머티리얼 리소스 해제
    constexpr UINT NumViews = static_cast<UINT>(EMaterialTextureSlots::MTS_MAX);
    
    ID3D11ShaderResourceView* NullSRVs[NumViews] = { nullptr };
    ID3D11SamplerState* NullSamplers[NumViews] = { nullptr};
    
    Graphics->DeviceContext->PSSetShaderResources(0, NumViews, NullSRVs);
    Graphics->DeviceContext->PSSetSamplers(0, NumViews, NullSamplers);

    // for Gouraud shading
    ID3D11SamplerState* NullSampler[1] = { nullptr};
    Graphics->DeviceContext->VSSetShaderResources(0, 1, NullSRV);
    Graphics->DeviceContext->VSSetSamplers(0, 1, NullSampler);
    
    // SRV 해제
    ID3D11ShaderResourceView* NullSRVs2[14] = { nullptr };
    Graphics->DeviceContext->PSSetShaderResources(0, 14, NullSRVs2);

    // 상수버퍼 해제
    ID3D11Buffer* NullPSBuffer[8] = { nullptr };
    Graphics->DeviceContext->PSSetConstantBuffers(0, 8, NullPSBuffer);
    ID3D11Buffer* NullVSBuffer[2] = { nullptr };
    Graphics->DeviceContext->VSSetConstantBuffers(0, 2, NullVSBuffer);
}

void FStaticMeshRenderPass::ClearRenderArr()
{
    StaticMeshComponents.Empty();
}
