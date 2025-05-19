#pragma once
#include "IRenderPass.h"


struct FParticleRenderPass : public IRenderPass
{
    FParticleRenderPass();
    virtual ~FParticleRenderPass() override;
    
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage) override;

    virtual void PrepareRenderArr() override;
    
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void ClearRenderArr() override;

protected:
    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;
};
