#pragma once
#include "RenderPassBase.h"
#include "EngineBaseTypes.h"
#include "Container/Set.h"

#include "Define.h"
#include "Components/Light/PointLightComponent.h"

struct FStaticMeshRenderData;
class FShadowManager;
class FDXDShaderManager;
class UWorld;
class UMaterial;
class FEditorViewportClient;
class UStaticMeshComponent;
struct FStaticMaterial;
class FShadowRenderPass;

class FStaticMeshRenderPass : public FRenderPassBase
{
public:
    FStaticMeshRenderPass() = default;
    virtual ~FStaticMeshRenderPass() override = default;
    
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    
    void InitializeShadowManager(class FShadowManager* InShadowManager);
    
    virtual void PrepareRenderArr() override;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void ClearRenderArr() override;
    
    virtual void PrepareRenderState(const std::shared_ptr<FEditorViewportClient>& Viewport);

    virtual void RenderAllStaticMeshes(const std::shared_ptr<FEditorViewportClient>& Viewport);
    
    void UpdateLitUnlitConstant(int32 IsLit) const;

    void RenderPrimitive(FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int SelectedSubMeshIndex) const;
    
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT NumVertices) const;

    void RenderPrimitive(ID3D11Buffer* pVertexBuffer, UINT NumVertices, ID3D11Buffer* pIndexBuffer, UINT NumIndices) const;

    void CreateShader();
    
    void ChangeViewMode(EViewModeIndex ViewMode);
    
protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    
    FShadowManager* ShadowManager;
};
