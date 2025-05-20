#pragma once
#include "ParticleHelper.h"
#include "RenderPassBase.h"

class UParticleSystemComponent;

class FParticleSpriteRenderPass : public FRenderPassBase
{
public:
    FParticleSpriteRenderPass() = default;
    virtual ~FParticleSpriteRenderPass() override = default;

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage) override;
    virtual void PrepareRenderArr() override;
    virtual void ClearRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    TArray<UParticleSystemComponent*> ParticleComponents;
    
private:
    void DrawParticles();
    void ProcessParticles(const FDynamicSpriteEmitterReplayDataBase* ReplayData);
};
