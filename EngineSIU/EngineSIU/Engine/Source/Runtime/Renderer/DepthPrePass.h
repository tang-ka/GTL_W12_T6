#pragma once

#include "RenderPassBase.h"
#include "Container/Array.h"

struct FSkeletalMeshRenderData;
class UMaterial;
struct FStaticMaterial;
struct FStaticMeshRenderData;
class USkeletalMeshComponent;
class UStaticMeshComponent;

class FDepthPrePass : public FRenderPassBase
{
public:
    FDepthPrePass() = default;
    virtual ~FDepthPrePass() override = default;
    
    virtual void PrepareRenderArr() override;
    virtual void ClearRenderArr() override;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    
    void PrepareStaticMesh();
    void PrepareSkeletalMesh();

    void RenderStaticMesh();
    void RenderSkeletalMesh();
    
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
};

