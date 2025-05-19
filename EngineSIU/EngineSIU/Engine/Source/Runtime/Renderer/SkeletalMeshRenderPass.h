#pragma once
#include "SkeletalMeshRenderPassBase.h"

enum class EViewModeIndex : uint8;

class FSkeletalMeshRenderPass : public FSkeletalMeshRenderPassBase
{
protected:
    virtual void CreateResource();

    virtual void ReleaseResource();
    
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport);

    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void ChangeViewMode(EViewModeIndex ViewMode);

    void UpdateLitUnlitConstant(int32 IsLit) const;
};
