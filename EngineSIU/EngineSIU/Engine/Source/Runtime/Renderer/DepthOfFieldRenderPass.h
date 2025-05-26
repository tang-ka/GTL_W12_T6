#pragma once
#include "RenderPassBase.h"

#include "Define.h"

class FDepthOfFieldRenderPass : public FRenderPassBase
{
public:
    FDepthOfFieldRenderPass() = default;
    virtual ~FDepthOfFieldRenderPass() = default;

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    virtual void PrepareRenderArr() override;
    virtual void ClearRenderArr() override;
    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

private:
    void CreateShaders();

    void PrepareLayerPass(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void CleanUpLayerPass(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void PrepareMaxFilter_Near(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void CleanUpMaxFilter_Near(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void PrepareCoCBlur(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void CleanUpCoCBlur(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void PrepareBlur(const std::shared_ptr<FEditorViewportClient>& Viewport, bool bNear);
    void CleanUpBlur(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void PrepareComposite(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void CleanUpComposite(const std::shared_ptr<FEditorViewportClient>& Viewport);
};
