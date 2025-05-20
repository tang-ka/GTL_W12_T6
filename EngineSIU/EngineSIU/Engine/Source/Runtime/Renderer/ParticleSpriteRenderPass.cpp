#include "ParticleSpriteRenderPass.h"

#include "UnrealClient.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Engine/Engine.h"
#include "LevelEditor/SLevelEditor.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"
#include "Particles/ParticleSystemComponent.h"

void FParticleSpriteRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManage);

    HRESULT hr = ShaderManager->AddVertexShader(L"ParticleSpriteVertexShader", L"Shaders/ParticleSpriteVertexShader.hlsl", "main");
    if (FAILED(hr))
    {
        UE_LOG(ELogLevel::Error, TEXT("Failed to create ParticleSpriteVertexShader shader!"));
    }

    hr = ShaderManager->AddPixelShader(L"ParticleSpritePixelShader", L"Shaders/ParticleSpritePixelShader.hlsl", "main");
    if (FAILED(hr))
    {
        UE_LOG(ELogLevel::Error, TEXT("Failed to create ParticleSpritePixelShader shader!"));
    }
}

void FParticleSpriteRenderPass::PrepareRenderArr()
{
    for (auto Iter : TObjectRange<UParticleSystemComponent>())
    {
        if (Iter->GetWorld() == GEngine->ActiveWorld)
        {
            // TODO: 스프라이트인 경우에만 추가
            Iter->UpdateDynamicData();
            ParticleComponents.Add(Iter);
        }
    }

    ParticleComponents.Sort(
        [](const UParticleSystemComponent* A, const UParticleSystemComponent* B)
        {
            const FVector LocA = A->GetComponentLocation();
            const FVector LocB = B->GetComponentLocation();
            const FVector LocCam = GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->GetCameraLocation();

            const float DistA = (LocCam - LocA).SquaredLength();
            const float DistB = (LocCam - LocB).SquaredLength();
            
            return DistA > DistB;
        }
    );
}

void FParticleSpriteRenderPass::ClearRenderArr()
{
    ParticleComponents.Empty();
}

void FParticleSpriteRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);

    DrawParticles();

    SpriteVertices.Sort(
    [Viewport](const FParticleSpriteVertex& A, const FParticleSpriteVertex& B)
    {
        const FVector LocA = A.Position;
        const FVector LocB = B.Position;
        const FVector LocCam = Viewport->GetCameraLocation();

        const float DistA = (LocCam - LocA).SquaredLength();
        const float DistB = (LocCam - LocB).SquaredLength();
            
        return DistA > DistB;
    });
    
    BufferManager->UpdateStructuredBuffer("ParticleSpriteInstanceBuffer", SpriteVertices);
    BufferManager->BindStructuredBufferSRV("ParticleSpriteInstanceBuffer", 0, EShaderStage::Vertex);

    Graphics->DeviceContext->DrawInstanced(6, SpriteVertices.Num(), 0, 0);
    
    CleanUpRender(Viewport);
}

void FParticleSpriteRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    constexpr EResourceType ResourceType = EResourceType::ERT_Scene;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    const FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    const FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(ResourceType);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    Graphics->DeviceContext->OMSetBlendState(Graphics->BlendState_AlphaBlend, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_DepthWriteDisabled, 1);

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(L"ParticleSpriteVertexShader");
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(L"ParticleSpritePixelShader");
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(nullptr);
}

void FParticleSpriteRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_Default, 1);
}

void FParticleSpriteRenderPass::DrawParticles()
{
    SpriteVertices.Empty();
    
    for (const auto PSC : ParticleComponents)
    {
        FParticleDynamicData* Particle = PSC->GetParticleDynamicData();
        if (Particle)
        {
            for (auto Emitter : Particle->DynamicEmitterDataArray)
            {
                FDynamicEmitterReplayDataBase ReplayData = Emitter->GetSource();

                if (ReplayData.eEmitterType == DET_Sprite)
                {
                    FDynamicSpriteEmitterDataBase* SpriteData = dynamic_cast<FDynamicSpriteEmitterDataBase*>(Emitter);
                    if (SpriteData)
                    {
                        const FDynamicSpriteEmitterReplayDataBase* SpriteParticleData = SpriteData->GetSourceData();
                        ProcessParticles(SpriteParticleData);
                    }
                }
            }
        }
    }
}

void FParticleSpriteRenderPass::ProcessParticles(const FDynamicSpriteEmitterReplayDataBase* ReplayData)
{
    // ReplayData
    if (ReplayData == nullptr)
    {
        return;
    }

    // Sample code..
    SpriteVertices = {
        { FVector(0.f), 0.f, FVector(0.f), 0.f, FVector2D(1.f), 0.f, 0.f, FLinearColor(0.f, 0.f, 0.f, 1.f) },
        { FVector(1.f), 0.f, FVector(0.f), 0.f, FVector2D(0.5f), 0.f, 0.f, FLinearColor(0.f, 0.f, 0.f, 1.f) },
        { FVector(2.f), 0.f, FVector(0.f), 0.f, FVector2D(0.25f), 0.f, 0.f, FLinearColor(0.f, 0.f, 0.f, 1.f) },
    };
}
