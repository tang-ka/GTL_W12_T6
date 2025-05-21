#include "ParticleMeshRenderPass.h"

#include "ParticleHelper.h"
#include "RendererHelpers.h"
#include "UnrealClient.h"
#include "Components/Material/Material.h"
#include "D3D11RHI/DXDBufferManager.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "Engine/Engine.h"
#include "LevelEditor/SLevelEditor.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"

struct FRenderTargetRHI;
struct FMeshParticleInstanceVertex;
class UParticleSystemComponent;

void FParticleMeshRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManage);
}

void FParticleMeshRenderPass::PrepareRenderArr()
{
    for (auto Iter : TObjectRange<UParticleSystemComponent>())
    {
        if (Iter->GetWorld() == GEngine->ActiveWorld)
        {
            FParticleDynamicData* Particle = Iter->GetParticleDynamicData();
            if (Particle && !Particle->DynamicEmitterDataArray.IsEmpty())
            {
                for (auto Emitter : Particle->DynamicEmitterDataArray)
                {
                    const FDynamicEmitterReplayDataBase& ReplayData = Emitter->GetSource();

                    // TODO: 최적화 필요
                    if (ReplayData.eEmitterType == DET_Mesh)
                    {
                        ParticleComponents.Add(Iter);
                    }
                }
            }
        }
    }
}

void FParticleMeshRenderPass::ClearRenderArr()
{
    ParticleComponents.Empty();
}

void FParticleMeshRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);

    //DrawParticles();
    
    CleanUpRender(Viewport);
}

void FParticleMeshRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    constexpr EResourceType ResourceType = EResourceType::ERT_Scene;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    const FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    const FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(ResourceType);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_Default, 1);

    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(L"ParticleSpriteVertexShader");
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(L"CommonMeshShader");
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(nullptr);
    
    BufferManager->BindStructuredBufferSRV("ParticleMeshInstanceBuffer", 1, EShaderStage::Vertex);

    BufferManager->BindConstantBuffer(TEXT("FMaterialConstants"), 0, EShaderStage::Pixel);
    BufferManager->BindConstantBuffer(TEXT("FSubUVConstant"), 1, EShaderStage::Pixel);
}

void FParticleMeshRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    Graphics->DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_Default, 1);
}

void FParticleMeshRenderPass::DrawParticles()
{
    for (const auto PSC : ParticleComponents)
    {
        FParticleDynamicData* Particle = PSC->GetParticleDynamicData();
        if (Particle)
        {
            for (auto Emitter : Particle->DynamicEmitterDataArray)
            {
                const FDynamicEmitterReplayDataBase& ReplayData = Emitter->GetSource();

                if (ReplayData.eEmitterType == DET_Mesh)
                {
                    FDynamicSpriteEmitterDataBase* MeshData = dynamic_cast<FDynamicSpriteEmitterDataBase*>(Emitter);
                    if (MeshData)
                    {
                        const FDynamicSpriteEmitterReplayDataBase* SpriteParticleData = MeshData->GetSourceData();
                        ProcessParticles(SpriteParticleData);
                    }
                }
            }
        }
    }
}

void FParticleMeshRenderPass::ProcessParticles(const FDynamicSpriteEmitterReplayDataBase* ReplayData)
{
    // ReplayData
    if (ReplayData == nullptr)
    {
        return;
    }

    TArray<FMeshParticleInstanceVertex> SpriteVertices;
    
    const uint8* ParticleData = ReplayData->DataContainer.ParticleData;
    const int32 ParticleStride = ReplayData->ParticleStride;

    for (int32 i = 0; i < ReplayData->ActiveParticleCount; i++)
    {
        const uint8* ParticleBase = ParticleData + i * ParticleStride;
        
        DECLARE_PARTICLE_CONST(Particle, ParticleBase)
        FMeshParticleInstanceVertex MeshVertex = {};
        MeshVertex.Color = Particle.Color;

        SpriteVertices.Add(MeshVertex);
    }
    
    BufferManager->UpdateStructuredBuffer("ParticleSpriteInstanceBuffer", SpriteVertices);

    if (UMaterial* Material = ReplayData->MaterialInterface)
    {
        const FMaterialInfo& MaterialInfo = Material->GetMaterialInfo();
        
        MaterialUtils::UpdateMaterial(BufferManager, Graphics, MaterialInfo);
    }

    /*
    FSubUVConstant SubUVConstant = {
        FVector2D(0.0f, 0.0f),
        FVector2D(1.0f, 1.0f)   
    };
    BufferManager->UpdateConstantBuffer(TEXT("FSubUVConstant"), SubUVConstant);
    */

    Graphics->DeviceContext->DrawInstanced(6, SpriteVertices.Num(), 0, 0);
}
