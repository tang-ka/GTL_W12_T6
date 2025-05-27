#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>

#include "RenderPassBase.h"
#include "RenderResources.h"
#include "D3D11RHI/DXDBufferManager.h"
#include "Math/Transform.h"

class FDXDBufferManager;
class UWorld;
class FDXDShaderManager;
class USkeletalMeshComponent;
class UPhysicsAsset;
struct FKAggregateGeom;
struct FKBoxElem;
struct FKSphereElem;
struct FKSphylElem;

class FEditorRenderPass : public FRenderPassBase
{
public:
    void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager) override;
    void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    void PrepareRenderArr() override;
    void ClearRenderArr() override;

private:
    virtual void PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    virtual void CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport) override;
    
    FRenderResourcesDebug Resources = FRenderResourcesDebug();

private:
    void CreateShaders();
    void CreateBuffers();
    void CreateConstantBuffers();
    
    void LazyLoad();

    void BindRenderTarget(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void BindShaderResource(const std::wstring& VertexKey, const std::wstring& PixelKey, D3D_PRIMITIVE_TOPOLOGY Topology) const;
    void BindBuffers(const FDebugPrimitiveData& InPrimitiveData) const;
    
    void RenderPointlightInstanced(uint64 ShowFlag);
    void RenderSpotlightInstanced(uint64 ShowFlag);
    void RenderArrowInstanced();
    void RenderBoxInstanced(uint64 ShowFlag);
    void RenderSphereInstanced(uint64 ShowFlag);
    void RenderCapsuleInstanced(uint64 ShowFlag);

    void RenderPhysicsAssetDebug(UPhysicsAsset* PhysicsAsset, USkeletalMeshComponent* SkelMeshComp);
    void RenderPhysicsAssetsDebug(uint64 ShowFlag);

    TArray<FVector> GenerateUVSphereVertices(int32 Rings, int32 Sectors);
    TArray<uint32> GenerateUVSphereIndices(int32 Rings, int32 Sectors);

    void RenderBoxElements(const TArray<FKBoxElem>& BoxElems, const FTransform& BoneTransform, const FTransform& BaseTransform);
    void RenderInstancedBoxes(const TArray<FConstantBufferDebugBox>& BufferAll);
    void RenderSphereElements(const TArray<FKSphereElem>& SphereElems, const FTransform& BoneTransform, const FTransform& BaseTransform);
    void RenderInstancedSpheres(const TArray<FConstantBufferDebugSphere>& BufferAll);
    void RenderCapsuleElements(const TArray<FKSphylElem>& CapsuleElems, const FTransform& BoneTransform, const FTransform& BaseTransform);
    void RenderInstancedCapsules(const TArray<FConstantBufferDebugCapsule>& BufferAll);

    // Grid
    // void RenderGrid(std::shared_ptr<FEditorViewportClient> ActiveViewport);

    // Icon
    void RenderIcons(const UWorld* World, std::shared_ptr<FEditorViewportClient> ActiveViewport);   // 사용 X
    void UpdateTextureIcon(EIconType Type);

    static constexpr UINT32 ConstantBufferSizeBox = 100;
    static constexpr UINT32 ConstantBufferSizeSphere = 100;
    static constexpr UINT32 ConstantBufferSizeCone = 100;
    static constexpr UINT32 ConstantBufferSizeArrow = 100;
    static constexpr UINT32 ConstantBufferSizeCapsule = 100;
};
