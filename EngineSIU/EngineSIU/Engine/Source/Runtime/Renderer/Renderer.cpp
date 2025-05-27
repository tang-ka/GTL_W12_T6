#include "Renderer.h"

#include <array>
#include "World/World.h"
#include "UnrealClient.h"
#include "UnrealEd/EditorViewportClient.h"
#include "PropertyEditor/ShowFlags.h"
// Helper
#include "ParticleHelper.h"
// Managers
#include "D3D11RHI/DXDShaderManager.h"
#include "ShadowManager.h"

// PreScene Passes
#include "DepthPrePass.h"
#include "TileLightCullingPass.h"
#include "UpdateLightBufferPass.h"
#include "ShadowRenderPass.h"
// Opaque Passes
#include "OpaqueRenderPass.h"
#include "ParticleMeshRenderPass.h"
// EditorDepthElement Passes
#include "EditorRenderPass.h"
#include "LineRenderPass.h"
// Translucent Passes
#include "TranslucentRenderPass.h"
#include "ParticleSpriteRenderPass.h"
#include "WorldBillboardRenderPass.h"
#include "EditorBillboardRenderPass.h"
// EditorOverlay Passes
#include "GizmoRenderPass.h"
// Final Passes
#include "PostProcessRenderPass.h"
#include "CompositingPass.h"
#include "SlateRenderPass.h"

// Console
#include "Stats/Stats.h"
#include "Stats/GPUTimingManager.h"

//------------------------------------------------------------------------------
// 초기화 및 해제 관련 함수
//------------------------------------------------------------------------------
void FRenderer::Initialize(FGraphicsDevice* InGraphics, FDXDBufferManager* InBufferManager, FGPUTimingManager* InGPUTimingManager)
{
    Graphics = InGraphics;
    BufferManager = InBufferManager;
    GPUTimingManager = InGPUTimingManager;

    ShaderManager = new FDXDShaderManager(Graphics->Device);
    ShadowManager = new FShadowManager();

    CreateConstantBuffers();
    CreateCommonShader();

    // PreScene Passes
    DepthPrePass = AddRenderPass<FDepthPrePass>();
    TileLightCullingPass = AddRenderPass<FTileLightCullingPass>();
    UpdateLightBufferPass = AddRenderPass<FUpdateLightBufferPass>();
    ShadowRenderPass = AddRenderPass<FShadowRenderPass>();
    {
        assert(ShadowManager->Initialize(Graphics, BufferManager) && "ShadowManager Initialize Failed");
        ShadowRenderPass->InitializeShadowManager(ShadowManager);
    }

    // Opaque Passes
    OpaqueRenderPass = AddRenderPass<FOpaqueRenderPass>();
    ParticleMeshRenderPass = AddRenderPass<FParticleMeshRenderPass>();

    // EditorDepthElement Passes
    EditorRenderPass = AddRenderPass<FEditorRenderPass>();
    LineRenderPass = AddRenderPass<FLineRenderPass>();

    // Translucent Passes
    TranslucentRenderPass = AddRenderPass<FTranslucentRenderPass>();
    ParticleSpriteRenderPass = AddRenderPass<FParticleSpriteRenderPass>();
    WorldBillboardRenderPass = AddRenderPass<FWorldBillboardRenderPass>();
    EditorBillboardRenderPass = AddRenderPass<FEditorBillboardRenderPass>();

    // EditorOverlay Passes
    GizmoRenderPass = AddRenderPass<FGizmoRenderPass>();

    // Final Passes
    PostProcessRenderPass = AddRenderPass<FPostProcessRenderPass>();
    CompositingPass = AddRenderPass<FCompositingPass>();
    SlateRenderPass = AddRenderPass<FSlateRenderPass>();

    for (IRenderPass* RenderPass : RenderPasses)
    {
        RenderPass->Initialize(BufferManager, Graphics, ShaderManager);
    }
}

void FRenderer::Release()
{
    delete ShaderManager;
    delete ShadowManager;

    for (const IRenderPass* RenderPass : RenderPasses)
    {
        delete RenderPass;
    }
}

//------------------------------------------------------------------------------
// 사용하는 모든 상수 버퍼 생성
//------------------------------------------------------------------------------
void FRenderer::CreateConstantBuffers()
{
    // 공통
    BufferManager->CreateBufferGeneric<FCameraConstantBuffer>("FCameraConstantBuffer", nullptr, sizeof(FCameraConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Light
    BufferManager->CreateBufferGeneric<FLightInfoBuffer>("FLightInfoBuffer", nullptr, sizeof(FLightInfoBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FLitUnlitConstants>("FLitUnlitConstants", nullptr, sizeof(FLitUnlitConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Shadow
    BufferManager->CreateBufferGeneric<FPointLightGSBuffer>("FPointLightGSBuffer", nullptr, sizeof(FPointLightGSBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FCascadeConstantBuffer>("FCascadeConstantBuffer", nullptr, sizeof(FCascadeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FIsShadowConstants>("FIsShadowConstants", nullptr, sizeof(FIsShadowConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FShadowConstantBuffer>("FShadowConstantBuffer", nullptr, sizeof(FShadowConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Mesh
    BufferManager->CreateBufferGeneric<FObjectConstantBuffer>("FObjectConstantBuffer", nullptr, sizeof(FObjectConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FMaterialConstants>("FMaterialConstants", nullptr, sizeof(FMaterialConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Texture
    BufferManager->CreateBufferGeneric<FSubMeshConstants>("FSubMeshConstants", nullptr, sizeof(FSubMeshConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FTextureUVConstants>("FTextureConstants", nullptr, sizeof(FTextureUVConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FSubUVConstant>("FSubUVConstant", nullptr, sizeof(FSubUVConstant), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Viewport
    BufferManager->CreateBufferGeneric<FShowFlagConstants>("FShowFlagConstants", nullptr, sizeof(FShowFlagConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FViewportSize>("FViewportSize", nullptr, sizeof(FViewportSize), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FScreenConstants>("FScreenConstants", nullptr, sizeof(FScreenConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Skeletal Mesh
    BufferManager->CreateBufferGeneric<FCPUSkinningConstants>("FCPUSkinningConstants", nullptr, sizeof(FCPUSkinningConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateStructuredBufferGeneric<FMatrix>("BoneBuffer", nullptr, MaxBoneNum, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Particle
    BufferManager->CreateStructuredBufferGeneric<FParticleSpriteVertex>("ParticleSpriteInstanceBuffer", nullptr, MaxParticleInstanceNum, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateStructuredBufferGeneric<FMeshParticleInstanceVertex>("ParticleMeshInstanceBuffer", nullptr, MaxParticleInstanceNum, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    // Post Processing
    BufferManager->CreateBufferGeneric<FFogConstants>("FFogConstants", nullptr, sizeof(FFogConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FDepthOfFieldConstants>("FDepthOfFieldConstants", nullptr, sizeof(FDepthOfFieldConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

    
    // TODO: 함수로 분리
    ID3D11Buffer* ObjectBuffer = BufferManager->GetConstantBuffer(TEXT("FObjectConstantBuffer"));
    ID3D11Buffer* CameraConstantBuffer = BufferManager->GetConstantBuffer(TEXT("FCameraConstantBuffer"));
    Graphics->DeviceContext->VSSetConstantBuffers(12, 1, &ObjectBuffer);
    Graphics->DeviceContext->VSSetConstantBuffers(13, 1, &CameraConstantBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(12, 1, &ObjectBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(13, 1, &CameraConstantBuffer);
}

void FRenderer::ReleaseConstantBuffer() const
{
    BufferManager->ReleaseConstantBuffer();
    BufferManager->ReleaseStructuredBuffer();
}

void FRenderer::CreateCommonShader() const
{
    D3D11_INPUT_ELEMENT_DESC StaticMeshLayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HRESULT hr = ShaderManager->AddVertexShaderAndInputLayout(L"StaticMeshVertexShader", L"Shaders/StaticMeshVertexShader.hlsl", "mainVS", StaticMeshLayoutDesc, ARRAYSIZE(StaticMeshLayoutDesc));
    if (FAILED(hr))
    {
        return;
    }

    D3D11_INPUT_ELEMENT_DESC SkeletalMeshLayoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = ShaderManager->AddVertexShaderAndInputLayout(L"SkeletalMeshVertexShader", L"Shaders/SkeletalMeshVertexShader.hlsl", "mainVS", SkeletalMeshLayoutDesc, ARRAYSIZE(SkeletalMeshLayoutDesc));
    if (FAILED(hr))
    {
        return;
    }
    
#pragma region UberShader
    D3D_SHADER_MACRO DefinesGouraud[] =
    {
        { GOURAUD, "1" },
        { nullptr, nullptr }
    };
    hr = ShaderManager->AddVertexShaderAndInputLayout(L"GOURAUD_StaticMeshVertexShader", L"Shaders/StaticMeshVertexShader.hlsl", "mainVS", StaticMeshLayoutDesc, ARRAYSIZE(StaticMeshLayoutDesc), DefinesGouraud);
    if (FAILED(hr))
    {
        return;
    }
    
    hr = ShaderManager->AddVertexShaderAndInputLayout(L"GOURAUD_SkeletalMeshVertexShader", L"Shaders/SkeletalMeshVertexShader.hlsl", "mainVS", SkeletalMeshLayoutDesc, ARRAYSIZE(SkeletalMeshLayoutDesc), DefinesGouraud);
    if (FAILED(hr))
    {
        return;
    }
#pragma endregion UberShader

    hr = ShaderManager->AddVertexShader(L"FullScreenQuadVertexShader", L"Shaders/FullScreenQuadVertexShader.hlsl", "main");
    if (FAILED(hr))
    {
        return;
    }
}

void FRenderer::PrepareRender(FViewportResource* ViewportResource) const
{
    // Setup Viewport
    Graphics->DeviceContext->RSSetViewports(1, &ViewportResource->GetD3DViewport());

    ViewportResource->ClearDepthStencils(Graphics->DeviceContext);
    ViewportResource->ClearRenderTargets(Graphics->DeviceContext);

    PrepareRenderPass();
}

void FRenderer::PrepareRenderPass() const
{
    for (IRenderPass* RenderPass : RenderPasses)
    {
        RenderPass->PrepareRenderArr();
    }
}

void FRenderer::ClearRenderArr() const
{
    for (IRenderPass* RenderPass : RenderPasses)
    {
        RenderPass->ClearRenderArr();
    }
}

void FRenderer::UpdateCommonBuffer(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    FCameraConstantBuffer CameraConstantBuffer;
    CameraConstantBuffer.ViewMatrix = Viewport->GetViewMatrix();
    CameraConstantBuffer.InvViewMatrix = FMatrix::Inverse(CameraConstantBuffer.ViewMatrix);
    CameraConstantBuffer.ProjectionMatrix = Viewport->GetProjectionMatrix();
    CameraConstantBuffer.InvProjectionMatrix = FMatrix::Inverse(CameraConstantBuffer.ProjectionMatrix);
    CameraConstantBuffer.ViewLocation = Viewport->GetCameraLocation();
    CameraConstantBuffer.NearClip = Viewport->GetCameraNearClip();
    CameraConstantBuffer.FarClip = Viewport->GetCameraFarClip();
    BufferManager->UpdateConstantBuffer("FCameraConstantBuffer", CameraConstantBuffer);
}

void FRenderer::BeginRender(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    if (!ViewportResource)
    {
        return;
    }

    UpdateCommonBuffer(Viewport);
    
    PrepareRender(ViewportResource);
}

void FRenderer::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    if (!GPUTimingManager || !GPUTimingManager->IsInitialized())
    {
        return;
    }
    
    /**
     * 각 렌더 패스의 시작과 끝은 필요한 리소스를 바인딩하고 해제하는 것까지입니다.
     * 다음에 작동할 렌더 패스에서는 이전에 사용했던 리소스들을 충돌 없이 바인딩 할 수 있어야 한다는 의미입니다.
     * e.g.
     *   1번 렌더 패스: 여기에서 사용했던 RTV를 마지막에 해제함으로써, 해당 RTV와 연결된 텍스처를 쉐이더 리소스로 사용할 수 있습니다.
     *   2번 렌더 패스: 1번 렌더 패스에서 렌더한 결과 텍스처를 쉐이더 리소스로 사용할 수 있습니다.
     *
     * 경우에 따라(연속적인 렌더 패스에서 동일한 리소스를 사용하는 경우) 바인딩 및 해제 작업을 생략하는 것도 가능하지만,
     * 다음 전제 조건을 지켜주어야 합니다.
     *   1. 렌더 패스는 엄격하게 순차적으로 실행됨
     *   2. 렌더 타겟의 생명주기와 용도가 명확함
     *   3. RTV -> SRV 전환 타이밍이 정확히 지켜짐
     */
    QUICK_SCOPE_CYCLE_COUNTER(Renderer_Render_CPU)
    QUICK_GPU_SCOPE_CYCLE_COUNTER(Renderer_Render_GPU, *GPUTimingManager)
    {
        BeginRender(Viewport);
        
        RenderPreScene(Viewport);
        RenderOpaque(Viewport);
        RenderEditorDepthElement(Viewport);
        RenderTranslucent(Viewport);
        RenderEditorOverlay(Viewport);
        RenderPostProcess(Viewport);

        RenderFinalResult(Viewport);

        EndRender();
    }
}

void FRenderer::RenderPreScene(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    const uint64 ShowFlag = Viewport->GetShowFlag();
    if (ShowFlag & (EEngineShowFlags::SF_Primitives | EEngineShowFlags::SF_SkeletalMesh))
    {
        if (DepthPrePass) // Depth Pre Pass : 렌더타겟 nullptr 및 렌더 후 복구
        {
            QUICK_SCOPE_CYCLE_COUNTER(DepthPrePass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(DepthPrePass_GPU, *GPUTimingManager)
            DepthPrePass->Render(Viewport);
        }

        // Added Compute Shader Pass
        if (TileLightCullingPass)
        {
            {
                QUICK_SCOPE_CYCLE_COUNTER(TileLightCulling_CPU)
                QUICK_GPU_SCOPE_CYCLE_COUNTER(TileLightCulling_GPU, *GPUTimingManager)
                TileLightCullingPass->Render(Viewport);
            }

            // 이후 패스에서 사용할 수 있도록 리소스 생성
            // @todo UpdateLightBuffer에서 병목 발생 -> 필요한 라이트에 대하여만 업데이트 필요, Tiled Culling으로 GPU->CPU 전송은 주객전도
            UpdateLightBufferPass->SetLightData(
                TileLightCullingPass->GetPointLights(),
                TileLightCullingPass->GetSpotLights(),
                TileLightCullingPass->GetPerTilePointLightIndexMaskBufferSRV(),
                TileLightCullingPass->GetPerTileSpotLightIndexMaskBufferSRV()
            );

            {
                QUICK_SCOPE_CYCLE_COUNTER(UpdateLightBufferPass_CPU)
                QUICK_GPU_SCOPE_CYCLE_COUNTER(UpdateLightBufferPass_GPU, *GPUTimingManager)
                UpdateLightBufferPass->Render(Viewport);
            }
        }

        if (Viewport->GetViewMode() != EViewModeIndex::VMI_Unlit)
        {
            ShadowRenderPass->SetLightData(TileLightCullingPass->GetPointLights(), TileLightCullingPass->GetSpotLights());
            {
                QUICK_SCOPE_CYCLE_COUNTER(ShadowPass_CPU)
                QUICK_GPU_SCOPE_CYCLE_COUNTER(ShadowPass_GPU, *GPUTimingManager)
                ShadowRenderPass->Render(Viewport);
            }
            ShadowManager->BindResourcesForSampling();
        }
    }
}

void FRenderer::RenderOpaque(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    const uint64 ShowFlag = Viewport->GetShowFlag();
    
    if (ShowFlag & (EEngineShowFlags::SF_Primitives | EEngineShowFlags::SF_SkeletalMesh))
    {
        {
            QUICK_SCOPE_CYCLE_COUNTER(OpaquePass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(OpaquePass_GPU, *GPUTimingManager)
            OpaqueRenderPass->Render(Viewport);
        }
    }
    
    if (ShowFlag & EEngineShowFlags::SF_Particles)
    {
        {
            QUICK_SCOPE_CYCLE_COUNTER(ParticleMeshPass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(ParticleMeshPass_GPU, *GPUTimingManager)
            ParticleMeshRenderPass->Render(Viewport);
        }
    }
    
    // TODO: 이 시점에서 씬 뎁스 스텐실 버퍼를 복사해두면, 에디터 요소가 없는 순수한 뎁스 버퍼를 확보할 수 있음.
}

void FRenderer::RenderEditorDepthElement(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    if (GEngine->ActiveWorld->WorldType != EWorldType::PIE)
    {
        {
            QUICK_SCOPE_CYCLE_COUNTER(EditorRenderPass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(EditorRenderPass_GPU, *GPUTimingManager)
            EditorRenderPass->Render(Viewport); // TODO: 임시로 이전에 작성되었던 와이어 프레임 렌더 패스이므로, 이후 개선 필요.
        }
        {
            QUICK_SCOPE_CYCLE_COUNTER(LinePass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(LinePass_GPU, *GPUTimingManager)
            LineRenderPass->Render(Viewport); // 기존 뎁스를 그대로 사용하지만 뎁스를 클리어하지는 않음
        }
    }
}

void FRenderer::RenderTranslucent(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    const uint64 ShowFlag = Viewport->GetShowFlag();
    
    if (ShowFlag & (EEngineShowFlags::SF_Primitives | EEngineShowFlags::SF_SkeletalMesh))
    {
        {
            QUICK_SCOPE_CYCLE_COUNTER(TranslucentPass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(TranslucentPass_GPU, *GPUTimingManager)
            TranslucentRenderPass->Render(Viewport);
        }
    }

    if (ShowFlag & EEngineShowFlags::SF_Particles)
    {
        {
            QUICK_SCOPE_CYCLE_COUNTER(ParticleSpritePass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(ParticleSpritePass_GPU, *GPUTimingManager)
            ParticleSpriteRenderPass->Render(Viewport);
        }
    }
    
    if (ShowFlag & EEngineShowFlags::SF_BillboardText)
    {
        {
            // Render World Billboard
            QUICK_SCOPE_CYCLE_COUNTER(WorldBillboardPass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(WorldBillboardPass_GPU, *GPUTimingManager)
            WorldBillboardRenderPass->Render(Viewport);
        }
        if (GEngine->ActiveWorld->WorldType != EWorldType::PIE)
        {
            // Render Editor Billboard
            QUICK_SCOPE_CYCLE_COUNTER(EditorBillboardPass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(EditorBillboardPass_GPU, *GPUTimingManager)
            EditorBillboardRenderPass->Render(Viewport);
        }
    }
}

void FRenderer::RenderEditorOverlay(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    if (GEngine->ActiveWorld->WorldType != EWorldType::PIE)
    {
        {
            QUICK_SCOPE_CYCLE_COUNTER(GizmoPass_CPU)
            QUICK_GPU_SCOPE_CYCLE_COUNTER(GizmoPass_GPU, *GPUTimingManager)
            GizmoRenderPass->Render(Viewport); // 기존 뎁스를 SRV로 전달해서 샘플 후 비교하기 위해 기즈모 전용 DSV 사용
        }
    }
}

void FRenderer::RenderPostProcess(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    const EViewModeIndex ViewMode = Viewport->GetViewMode();

    if (ViewMode < EViewModeIndex::VMI_Unlit)
    {
        QUICK_SCOPE_CYCLE_COUNTER(PostProcessPass_CPU)
        QUICK_GPU_SCOPE_CYCLE_COUNTER(PostProcessPass_GPU, *GPUTimingManager)
        PostProcessRenderPass->Render(Viewport);
    }
}

void FRenderer::RenderFinalResult(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    {
        // Compositing: 위에서 렌더한 결과들을 하나로 합쳐서 뷰포트의 최종 이미지를 만드는 작업
        QUICK_SCOPE_CYCLE_COUNTER(CompositingPass_CPU)
        QUICK_GPU_SCOPE_CYCLE_COUNTER(CompositingPass_GPU, *GPUTimingManager)
        CompositingPass->Render(Viewport);
    }
}

void FRenderer::EndRender() const
{
    ClearRenderArr();
    ShaderManager->ReloadAllShaders(); 
}

void FRenderer::RenderViewport(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    QUICK_SCOPE_CYCLE_COUNTER(SlatePass_CPU)
    QUICK_GPU_SCOPE_CYCLE_COUNTER(SlatePass_GPU, *GPUTimingManager)
    SlateRenderPass->Render(Viewport);
}
