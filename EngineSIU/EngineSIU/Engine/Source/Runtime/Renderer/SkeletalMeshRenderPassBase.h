#pragma once
#include "RenderPassBase.h"
#include "Container/Array.h"
#include "D3D11RHI/DXDShaderManager.h"

class FShadowManager;
class USkeletalMesh;
class UMaterial;
struct FSkeletalMeshRenderData;
class USkeletalMeshComponent;

struct FMatrix;
struct FVector4;
struct FStaticMaterial;
struct FStaticMeshRenderData;
struct ID3D11Buffer;

class FSkeletalMeshRenderPassBase : public FRenderPassBase
{
public:
    FSkeletalMeshRenderPassBase() = default;
    virtual ~FSkeletalMeshRenderPassBase() override = default;

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;

    void InitializeShadowManager(FShadowManager* InShadowManager);

    virtual void PrepareRenderArr() override;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void ClearRenderArr() override;

protected:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void Render_Internal(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void RenderAllSkeletalMeshes(const std::shared_ptr<FEditorViewportClient>& Viewport);

    void RenderSkeletalMesh(const FSkeletalMeshRenderData* RenderData) const;

    void RenderSkeletalMesh(ID3D11Buffer* Buffer, UINT VerticesNum) const;

    void RenderSkeletalMesh(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, UINT IndicesNum) const;

    FShadowManager* ShadowManager;

    TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
};
