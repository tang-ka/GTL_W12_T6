#include "ParticleSpriteRenderPass.h"

void FParticleSpriteRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManage);
}

void FParticleSpriteRenderPass::PrepareRenderArr()
{
}

void FParticleSpriteRenderPass::ClearRenderArr()
{
}

void FParticleSpriteRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);

    
    
    CleanUpRender(Viewport);
}

void FParticleSpriteRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FParticleSpriteRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}
