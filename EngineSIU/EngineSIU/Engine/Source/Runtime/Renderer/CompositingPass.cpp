#include "CompositingPass.h"

#include <array>

#include "Define.h"
#include "DepthOfFieldRenderPass.h"
#include "RendererHelpers.h"
#include "UnrealClient.h"
#include "PropertyEditor/ShowFlags.h"
#include "UnrealEd/EditorViewportClient.h"

void FCompositingPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManage;

    ShaderManager->AddVertexShader(L"Compositing", L"Shaders/CompositingShader.hlsl", "mainVS");
    ShaderManager->AddPixelShader(L"Compositing", L"Shaders/CompositingShader.hlsl", "mainPS");

    ShowFlagBuffer = BufferManager->GetConstantBuffer("FShowFlagConstants");

    BufferManager->CreateBufferGeneric<FGammaConstants>("FGammaConstants", nullptr, sizeof(FGammaConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void FCompositingPass::PrepareRenderArr()
{
}

void FCompositingPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    // Setup
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    if (!ViewportResource)
    {
        return;
    }

    const EResourceType ResourceType = EResourceType::ERT_Compositing; 
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);

    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_Scene)->SRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Translucent), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_Translucent)->SRV);

    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_PostProcess), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_DoF_Compositing)->SRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Editor), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_Editor)->SRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_EditorOverlay), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_EditorOverlay)->SRV);

    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_CameraEffect), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_PP_CameraEffect)->SRV);
    
    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, nullptr);
    Graphics->DeviceContext->ClearRenderTargetView(RenderTargetRHI->RTV, ViewportResource->GetClearColor(ResourceType).data());

    Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //Graphics->DeviceContext->PSSetSamplers(0, 1, &Graphics->SamplerState_PointWrap);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &Graphics->SamplerState_PointClamp);

    // 버퍼 바인딩
    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &ShowFlagBuffer);
    
    // Update Constant Buffer
    FShowFlagConstants ShowFlagConstantData = {};
    ShowFlagConstantData.ShowFlag = static_cast<uint32>(Viewport->GetShowFlag());
    BufferManager->UpdateConstantBuffer<FShowFlagConstants>("FShowFlagConstants", ShowFlagConstantData);

    BufferManager->BindConstantBuffer(TEXT("FGammaConstants"), 1, EShaderStage::Pixel);

    FGammaConstants GammaConstantData = {};
    GammaConstantData.GammaValue = GammaValue;
    BufferManager->UpdateConstantBuffer<FGammaConstants>("FGammaConstants", GammaConstantData);

    // Render
    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(L"FullScreenQuadVertexShader");
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(L"Compositing");
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(nullptr);
    Graphics->DeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    
    Graphics->DeviceContext->Draw(6, 0);

    // Finish
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    // Clear
    ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_PostProcess), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_EditorOverlay), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_CameraEffect), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Debug), 1, NullSRV);
    // Clear Debug SRV Resources
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_DepthOfField_LayerInfo), 1, NullSRV);
}

void FCompositingPass::ClearRenderArr()
{
}

void FCompositingPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FCompositingPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}
