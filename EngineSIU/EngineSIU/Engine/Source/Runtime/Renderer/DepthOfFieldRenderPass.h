#pragma once
#include "RenderPassBase.h"
#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/DXDBufferManager.h"

class FDepthOfFieldRenderPass : public FRenderPassBase
{
public:
    FDepthOfFieldRenderPass() = default;
    virtual ~FDepthOfFieldRenderPass() override = default;
    
    virtual void PrepareRenderArr() override {}
    virtual void ClearRenderArr() override {}

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    
    // DoF 파라미터 Getter/Setter
    float GetFocusDepth() const { return FocusDepth; }
    float GetFocusRange() const { return FocusRange; }
    
    void SetFocusDepth(float InFocusDepth) { FocusDepth = InFocusDepth; }
    void SetFocusRange(float InFocusRange) { FocusRange = InFocusRange; }
    
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
    
    // DoF 파라미터를 위한 멤버 변수
    float FocusDepth = 10.0f;
    float FocusRange = 5.0f;
    float MaxBlurAmount = 1.0f;
};
