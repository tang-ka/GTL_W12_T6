#include "DepthOfFieldRenderPass.h"
#include "Engine/Engine.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Engine/World/World.h"
#include "Classes/Camera/PlayerCameraManager.h"
#include "Renderer/ShaderConstants.h"
#include "RendererHelpers.h"
#include "UnrealClient.h"
#include "PropertyEditor/ShowFlags.h"

void FDepthOfFieldRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManager);
    
    CreateShader();
    CreateBlendState();
    CreateSampler();
}

void FDepthOfFieldRenderPass::CreateShader()
{
    ShaderManager->AddVertexShader(L"DepthOfFieldVertexShader", L"Shaders/DepthOfFieldShader.hlsl", "mainVS");
    ShaderManager->AddPixelShader(L"DepthOfFieldPixelShader", L"Shaders/DepthOfFieldShader.hlsl", "mainPS");
    VertexShader = ShaderManager->GetVertexShaderByKey(L"DepthOfFieldVertexShader");
    PixelShader = ShaderManager->GetPixelShaderByKey(L"DepthOfFieldPixelShader");
    BufferManager->CreateBufferGeneric<FConstantBufferDepthOfField>("DepthOfFieldConstantBuffer", nullptr, sizeof(FConstantBufferDepthOfField), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void FDepthOfFieldRenderPass::UpdateShader()
{
    VertexShader = ShaderManager->GetVertexShaderByKey(L"DepthOfFieldVertexShader");
    PixelShader = ShaderManager->GetPixelShaderByKey(L"DepthOfFieldPixelShader");
}

void FDepthOfFieldRenderPass::CreateBlendState()
{
    D3D11_BLEND_DESC BlendDesc = {};
    BlendDesc.AlphaToCoverageEnable = FALSE;
    BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable = TRUE;
    BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Graphics->Device->CreateBlendState(&BlendDesc, &BlendState);
}

void FDepthOfFieldRenderPass::CreateSampler()
{
    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    Graphics->Device->CreateSamplerState(&SamplerDesc, &Sampler);
}

void FDepthOfFieldRenderPass::PrepareRenderState()
{
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);

    Graphics->DeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    Graphics->DeviceContext->IASetInputLayout(nullptr);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);

    Graphics->DeviceContext->PSSetSamplers(0, 1, &Sampler);
}

void FDepthOfFieldRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    const uint64 ShowFlag = Viewport->GetShowFlag();
    if (!(ShowFlag & static_cast<uint64>(EEngineShowFlags::SF_DoF)))
    {
        return;
    }

    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    const EResourceType ResourceType = EResourceType::ERT_PP_DoF;
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);

    // Set up depth texture and scene texture as shader resources
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_SceneDepth), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_SceneDepth)->SRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_Scene)->SRV);

    // @todo 하드코딩 제거
    // Set up DoF parameters with much more extreme values for very visible effect
    FConstantBufferDepthOfField DoFParams;
    DoFParams.FocusDepth = 10.0;
    DoFParams.FocusRange = 5.0f;
    DoFParams.MaxBlurAmount = 2.0f;
    DoFParams.Padding = 0.0f;
    
    // If PlayerCameraManager exists, we could get values from there
    if (GEngine->ActiveWorld->GetPlayerController() && GEngine->ActiveWorld->GetPlayerController()->PlayerCameraManager)
    {
        // Note: These properties would need to be added to PlayerCameraManager
        // DoFParams.FocusDepth = GEngine->ActiveWorld->GetPlayerController()->PlayerCameraManager->DoFFocusDepth;
        // DoFParams.FocusRange = GEngine->ActiveWorld->GetPlayerController()->PlayerCameraManager->DoFFocusRange;
        // DoFParams.MaxBlurAmount = GEngine->ActiveWorld->GetPlayerController()->PlayerCameraManager->DoFMaxBlurAmount;
    }
    
    BufferManager->UpdateConstantBuffer<FConstantBufferDepthOfField>("DepthOfFieldConstantBuffer", DoFParams);
    BufferManager->BindConstantBuffer("DepthOfFieldConstantBuffer", 0, EShaderStage::Pixel);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, nullptr);
    Graphics->DeviceContext->OMSetBlendState(BlendState, nullptr, 0xffffffff);

    UpdateShader();
    PrepareRenderState();

    // Draw fullscreen quad
    Graphics->DeviceContext->Draw(6, 0);

    // Cleanup
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    
    // Clear shader resources
    ID3D11ShaderResourceView* NullSRVs[2] = { nullptr, nullptr };
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_SceneDepth), 1, NullSRVs);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, NullSRVs);
}

void FDepthOfFieldRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    // Nothing to do here for now
}

void FDepthOfFieldRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    // Nothing to do here for now
}
