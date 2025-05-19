#pragma once
#include "RenderPassBase.h"
#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/DXDBufferManager.h"

class FCameraEffectRenderPass : public FRenderPassBase
{
public:
    FCameraEffectRenderPass() = default;
    virtual ~FCameraEffectRenderPass() override = default;
    
    virtual void PrepareRenderArr() override {}
    virtual void ClearRenderArr() override {}

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    
    void CreateShader();
    void UpdateShader();
    void CreateBlendState();
    void CreateSampler();
    void PrepareRenderState();

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

private:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11BlendState* BlendState = nullptr;
    ID3D11SamplerState* Sampler = nullptr;
};
