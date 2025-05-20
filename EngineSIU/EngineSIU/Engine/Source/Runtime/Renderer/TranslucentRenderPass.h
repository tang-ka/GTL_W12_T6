#pragma once
#include "RenderPassBase.h"
#include "ParticleHelper.h"
#include "D3D11RHI/DXDShaderManager.h"

class UParticleSystemComponent;

struct FTranslucentRenderPass : public FRenderPassBase
{
    FTranslucentRenderPass() = default;
    virtual ~FTranslucentRenderPass() override = default;

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage) override;
    virtual void PrepareRenderArr() override;
    virtual void ClearRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

private:
    void DrawParticles();
    void ProcessSpriteParticle(const struct FDynamicSpriteEmitterReplayDataBase* ReplayData);
    void UpdateBuffers() const;
    void BindShaders() const;
    void BindTextures();
    
protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    TArray<UParticleSystemComponent*> ParticleSystemComponents;
    TArray<FParticleSpriteVertex> SpriteParticles;
};
