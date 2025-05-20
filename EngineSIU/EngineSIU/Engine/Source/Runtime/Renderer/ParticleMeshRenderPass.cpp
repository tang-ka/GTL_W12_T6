#include "ParticleMeshRenderPass.h"

void FParticleMeshRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManage);
}

void FParticleMeshRenderPass::PrepareRenderArr()
{
}

void FParticleMeshRenderPass::ClearRenderArr()
{
}

void FParticleMeshRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);
    
    CleanUpRender(Viewport);
}

void FParticleMeshRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FParticleMeshRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}
