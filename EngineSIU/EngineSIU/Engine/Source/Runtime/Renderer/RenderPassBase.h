#pragma once
#include "IRenderPass.h"
#include "Container/Array.h"
#include "Math/Matrix.h"
#include "Math/Vector4.h"


class USkeletalMeshComponent;
class UMaterial;
struct FStaticMaterial;
struct FSkeletalMeshRenderData;
struct FStaticMeshRenderData;

class FRenderPassBase : public IRenderPass 
{
public:
    FRenderPassBase();
    virtual ~FRenderPassBase() override;

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;

    virtual void PrepareRenderArr() override;
    virtual void ClearRenderArr() override;

    template <typename RenderPassType>
        requires std::derived_from<RenderPassType, IRenderPass>
    RenderPassType* AddRenderPass();

protected:
    void UpdateObjectConstant(const FMatrix& WorldMatrix, const FVector4& UUIDColor, bool bIsSelected) const;

    void RenderStaticMesh_Internal(const FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int32 SelectedSubMeshIndex);
    void RenderStaticMeshInstanced_Internal(const FStaticMeshRenderData* RenderData, int32 InstanceCount, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int32 SelectedSubMeshIndex);

    void RenderSkeletalMesh_Internal(const FSkeletalMeshRenderData* RenderData);

    void UpdateBones(const USkeletalMeshComponent* SkeletalMeshComponent);

    virtual void Release();
    
    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;

    TArray<IRenderPass*> ChildRenderPasses;
};

template <typename RenderPassType> requires std::derived_from<RenderPassType, IRenderPass>
RenderPassType* FRenderPassBase::AddRenderPass()
{
    RenderPassType* RenderPass = new RenderPassType();
    ChildRenderPasses.Add(RenderPass);
    return RenderPass;
}
