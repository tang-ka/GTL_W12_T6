#include "TranslucentRenderPass.h"

#include "UnrealClient.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"

void FTranslucentRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManage);
}

void FTranslucentRenderPass::PrepareRenderArr()
{
    
}

void FTranslucentRenderPass::ClearRenderArr()
{
    
}

void FTranslucentRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);
    
    CleanUpRender(Viewport);
}

void FTranslucentRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    const FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(EResourceType::ERT_Translucent);
    const FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    Graphics->DeviceContext->OMSetBlendState(Graphics->BlendState_AlphaBlend, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_DepthWriteDisabled, 1);
}

void FTranslucentRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}
