#pragma once

#include "RenderPassBase.h"

class FFogRenderPass;
class FDepthOfFieldRenderPass;
class FCameraEffectRenderPass;
class FPostProcessCompositingPass;

class FPostProcessRenderPass : public FRenderPassBase
{
public:
    FPostProcessRenderPass();
    virtual ~FPostProcessRenderPass() override = default;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    FFogRenderPass* FogRenderPass = nullptr;
    FDepthOfFieldRenderPass* DepthOfFieldRenderPass = nullptr;
    FCameraEffectRenderPass* CameraEffectRenderPass = nullptr;
    FPostProcessCompositingPass* PostProcessCompositingPass = nullptr;
};
