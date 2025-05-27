#pragma once
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Define.h"
#include "Container/Set.h"

#include "D3D11RHI/GraphicDevice.h"
#include "D3D11RHI/DXDBufferManager.h"

enum class EResourceType : uint8;

class UObject;
class UWorld;

class FViewportResource;
class FEditorViewportClient;

class FDXDShaderManager;
class FShadowManager;

class FGPUTimingManager;

class IRenderPass;
// PreScene Passes
class FDepthPrePass;
class FTileLightCullingPass;
class FUpdateLightBufferPass;
class FLightHeatMapRenderPass;
class FShadowRenderPass;
// Opaque Passes
class FOpaqueRenderPass;
class FParticleMeshRenderPass;
// EditorDepthElement Passes
class FEditorRenderPass;
class FLineRenderPass;
// Translucent Passes
class FTranslucentRenderPass;
class FParticleSpriteRenderPass;
class FWorldBillboardRenderPass;
class FEditorBillboardRenderPass;
// EditorOverlay Passes
class FGizmoRenderPass;
// Final Passes
class FPostProcessRenderPass;
class FCompositingPass;
class FSlateRenderPass;

class FRenderer
{
public:
    //==========================================================================
    // 초기화/해제 관련 함수
    //==========================================================================
    void Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, FGPUTimingManager* InGPUTimingManager);
    void Release();

    //==========================================================================
    // 렌더 패스 관련 함수
    //==========================================================================
    void Render(const std::shared_ptr<FEditorViewportClient>& Viewport);
    void RenderViewport(const std::shared_ptr<FEditorViewportClient>& Viewport) const; // TODO: 추후 RenderSlate로 변경해야함

protected:
    void BeginRender(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void UpdateCommonBuffer(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void PrepareRender(FViewportResource* ViewportResource) const;
    void PrepareRenderPass() const;

    void RenderPreScene(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void RenderOpaque(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void RenderEditorDepthElement(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void RenderTranslucent(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void RenderEditorOverlay(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void RenderPostProcess(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    void RenderFinalResult(const std::shared_ptr<FEditorViewportClient>& Viewport) const;
    
    void EndRender() const;
    void ClearRenderArr() const;
    
    //==========================================================================
    // 버퍼 생성/해제 함수 (템플릿 포함)
    //==========================================================================
public:
    // 상수 버퍼 생성/해제
    void CreateConstantBuffers();
    void ReleaseConstantBuffer() const;

    void CreateCommonShader() const;

public:
    FGraphicsDevice* Graphics;
    FDXDBufferManager* BufferManager;
    FDXDShaderManager* ShaderManager = nullptr;
    FShadowManager* ShadowManager = nullptr;

    FGPUTimingManager* GPUTimingManager = nullptr;

    // PreScene Passes
    FDepthPrePass* DepthPrePass = nullptr;
    FTileLightCullingPass* TileLightCullingPass = nullptr;
    FUpdateLightBufferPass* UpdateLightBufferPass = nullptr;
    FShadowRenderPass* ShadowRenderPass;
    // Opaque Passes
    FOpaqueRenderPass* OpaqueRenderPass = nullptr;
    FParticleMeshRenderPass* ParticleMeshRenderPass = nullptr;
    // EditorDepthElement Passes
    FEditorRenderPass* EditorRenderPass = nullptr;
    FLineRenderPass* LineRenderPass = nullptr;
    // Translucent Passes
    FTranslucentRenderPass* TranslucentRenderPass = nullptr;
    FParticleSpriteRenderPass* ParticleSpriteRenderPass = nullptr;
    FWorldBillboardRenderPass* WorldBillboardRenderPass = nullptr;
    FEditorBillboardRenderPass* EditorBillboardRenderPass = nullptr;
    // EditorOverlay Passes
    FGizmoRenderPass* GizmoRenderPass = nullptr;
    // Final Passes
    FPostProcessRenderPass* PostProcessRenderPass = nullptr;
    FCompositingPass* CompositingPass = nullptr;
    FSlateRenderPass* SlateRenderPass = nullptr;

private:
    template <typename RenderPassType>
        requires std::derived_from<RenderPassType, IRenderPass>
    RenderPassType* AddRenderPass();

    TArray<IRenderPass*> RenderPasses;

private:
    const int32 MaxBoneNum = 1024;
    const int32 MaxParticleInstanceNum = 1024;
};

template <typename RenderPassType> requires std::derived_from<RenderPassType, IRenderPass>
RenderPassType* FRenderer::AddRenderPass()
{
    RenderPassType* RenderPass = new RenderPassType();
    RenderPasses.Add(RenderPass);
    return RenderPass;
}
