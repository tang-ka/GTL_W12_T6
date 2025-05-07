#include "SkeletalMeshRenderPass.h"

#include "UnrealClient.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"

void FSkeletalMeshRenderPass::CreateResource()
{
    FSkeletalMeshRenderPassBase::CreateResource();

    HRESULT hr = ShaderManager->AddPixelShader(L"SkeletalMeshPixelShader", L"Shaders/StaticMeshPixelShader.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
}

void FSkeletalMeshRenderPass::ReleaseResource()
{
    FSkeletalMeshRenderPassBase::ReleaseResource();
}

void FSkeletalMeshRenderPass::PrepareRenderPass(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    const EViewModeIndex ViewMode = Viewport->GetViewMode();

    ChangeViewMode(ViewMode);
    
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    const EResourceType ResourceType = EResourceType::ERT_Scene;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(ResourceType);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    Graphics->DeviceContext->VSSetShaderResources(1, 1, &BoneSRV);

    TArray<FString> PSBufferKeys = {
        TEXT("FLightInfoBuffer"),
        TEXT("FMaterialConstants"),
        TEXT("FLitUnlitConstants"),
        TEXT("FSubMeshConstants"),
        TEXT("FTextureConstants"),
    };

    BufferManager->BindConstantBuffers(PSBufferKeys, 0, EShaderStage::Pixel);

    BufferManager->BindConstantBuffer(TEXT("FLightInfoBuffer"), 0, EShaderStage::Vertex);
    BufferManager->BindConstantBuffer(TEXT("FMaterialConstants"), 1, EShaderStage::Vertex);
    BufferManager->BindConstantBuffer(TEXT("FObjectConstantBuffer"), 12, EShaderStage::Vertex);
}

void FSkeletalMeshRenderPass::CleanUpRenderPass(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
    Graphics->DeviceContext->VSSetShaderResources(1, 1, NullSRV);
    
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void FSkeletalMeshRenderPass::ChangeViewMode(EViewModeIndex ViewMode)
{
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    
    switch (ViewMode)
    {
    case EViewModeIndex::VMI_Lit_Gouraud:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"GOURAUD_StaticMeshPixelShader");
        UpdateLitUnlitConstant(1);
        break;
    case EViewModeIndex::VMI_Lit_Lambert:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"LAMBERT_StaticMeshPixelShader");
        UpdateLitUnlitConstant(1);
        break;
    case EViewModeIndex::VMI_Lit_BlinnPhong:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"PHONG_StaticMeshPixelShader");
        UpdateLitUnlitConstant(1);
        break;
    case EViewModeIndex::VMI_LIT_PBR:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"PBR_StaticMeshPixelShader");
        UpdateLitUnlitConstant(1);
        break;
    case EViewModeIndex::VMI_Wireframe:
    case EViewModeIndex::VMI_Unlit:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"LAMBERT_StaticMeshPixelShader");
        UpdateLitUnlitConstant(0);
        break;
    case EViewModeIndex::VMI_SceneDepth:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShaderDepth");
        UpdateLitUnlitConstant(0);
        break;
    case EViewModeIndex::VMI_WorldNormal:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShaderWorldNormal");
        UpdateLitUnlitConstant(0);
        break;
    case EViewModeIndex::VMI_WorldTangent:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"StaticMeshPixelShaderWorldTangent");
        UpdateLitUnlitConstant(0);
        break;
    // HeatMap ViewMode 등
    default:
        VertexShader = ShaderManager->GetVertexShaderByKey(L"SkeletalMeshVertexShader");
        InputLayout = ShaderManager->GetInputLayoutByKey(L"SkeletalMeshVertexShader");
        PixelShader = ShaderManager->GetPixelShaderByKey(L"LAMBERT_StaticMeshPixelShader");
        UpdateLitUnlitConstant(1);
        break;
    }

    // Rasterizer
    Graphics->ChangeRasterizer(ViewMode);

    // Setup
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(InputLayout);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
}

void FSkeletalMeshRenderPass::UpdateLitUnlitConstant(int32 IsLit) const
{
    FLitUnlitConstants Data;
    Data.bIsLit = IsLit;
    BufferManager->UpdateConstantBuffer(TEXT("FLitUnlitConstants"), Data);
}
