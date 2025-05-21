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
    virtual ~FRenderPassBase() override = default;

    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage) override;

protected:
    void UpdateObjectConstant(const FMatrix& WorldMatrix, const FVector4& UUIDColor, bool bIsSelected) const;

    void RenderStaticMesh_Internal(const FStaticMeshRenderData* RenderData, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int32 SelectedSubMeshIndex);
    void RenderStaticMeshInstanced_Internal(const FStaticMeshRenderData* RenderData, int32 InstanceCount, TArray<FStaticMaterial*> Materials, TArray<UMaterial*> OverrideMaterials, int32 SelectedSubMeshIndex);

    void RenderSkeletalMesh_Internal(const FSkeletalMeshRenderData* RenderData);

    void UpdateBones(const USkeletalMeshComponent* SkeletalMeshComponent);
    
    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;
};
