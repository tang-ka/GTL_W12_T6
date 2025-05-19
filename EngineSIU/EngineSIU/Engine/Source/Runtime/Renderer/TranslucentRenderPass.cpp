#include "TranslucentRenderPass.h"

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
}

void FTranslucentRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}
