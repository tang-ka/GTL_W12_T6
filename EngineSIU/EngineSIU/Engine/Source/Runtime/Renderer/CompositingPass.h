
#pragma once
#include <memory>

#include "RenderPassBase.h"
#include "D3D11RHI/DXDShaderManager.h"

class FCompositingPass : public FRenderPassBase
{
public:
    FCompositingPass() = default;
    virtual ~FCompositingPass() override = default;
    
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage) override;
    
    virtual void PrepareRenderArr() override;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void ClearRenderArr() override;

    float GammaValue = 1.f;
    
protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    ID3D11SamplerState* Sampler;

    ID3D11Buffer* ViewModeBuffer;
};
