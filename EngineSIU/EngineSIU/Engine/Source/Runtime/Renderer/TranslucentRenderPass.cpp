#include "TranslucentRenderPass.h"

#include "UnrealClient.h"
#include "Engine/Engine.h"
#include "D3D11RHI/GraphicDevice.h"
#include "UnrealEd/EditorViewportClient.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/UObjectIterator.h"

void FTranslucentRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManage);
    
    // 셰이더 생성
    D3D11_INPUT_ELEMENT_DESC ParticleInstanceLayout[] = {
        //     // 데이터 슬롯 0: SV_VertexID를 위한 더미 버텍스 버퍼 (선택 사항, 또는 사용 안 함)
        //     // 데이터 슬롯 1: 인스턴스 데이터
        { "POSITION",      0, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD",      0, DXGI_FORMAT_R32_FLOAT,          1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // RelativeTime
        { "TEXCOORD",      1, DXGI_FORMAT_R32G32B32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // OldPosition
        { "TEXCOORD",      3, DXGI_FORMAT_R32G32_FLOAT,       1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // Size
        { "TEXCOORD",      4, DXGI_FORMAT_R32_FLOAT,          1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // Rotation
        { "TEXCOORD",      5, DXGI_FORMAT_R32_FLOAT,          1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // SubImageIndex
        { "COLOR",         0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
    };

    HRESULT hr = ShaderManager->AddVertexShaderAndInputLayout(L"CascadeParticle", L"Shaders/CascadeParticleShader.hlsl", "mainVS", ParticleInstanceLayout, ARRAYSIZE(ParticleInstanceLayout));
    if (FAILED(hr))
    {
        return;
    }
    
    hr = ShaderManager->AddPixelShader(L"CascadeParticle", L"Shaders/CascadeParticleShader.hlsl", "mainPS");
    if (FAILED(hr))
    {
        return;
    }
}

void FTranslucentRenderPass::PrepareRenderArr()
{
    ParticleSystemComponents.Empty();
    for (const auto Iter : TObjectRange<UParticleSystemComponent>())
    {
        if (Iter->GetWorld() == GEngine->ActiveWorld)
        {
            Iter->UpdateDynamicData();
            ParticleSystemComponents.Add(Iter);
        }
    }
}

void FTranslucentRenderPass::ClearRenderArr()
{
    ParticleSystemComponents.Empty();
}

void FTranslucentRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    PrepareRender(Viewport);

    DrawParticles();
    
    CleanUpRender(Viewport);
}

void FTranslucentRenderPass::DrawParticles()
{
    for (const auto PSC : ParticleSystemComponents)
    {
        // Update world matrix
        UpdateObjectConstant(PSC->GetWorldMatrix(), FVector4(), false);
        
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
                        ProcessSpriteParticle(SpriteParticleData);
                    }
                }
            }
        }
    }

    if (SpriteParticles.IsEmpty())
    {
        return;
    }

    UpdateBuffers();
    BindShaders();
    BindTextures();

    // Draw Call
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->DrawInstanced(6, SpriteParticles.Num(), 0, 0);
}

void FTranslucentRenderPass::ProcessSpriteParticle(const struct FDynamicSpriteEmitterReplayDataBase* ReplayData)
{
    if (ReplayData == nullptr)
    {
        return;
    }
    
    FParticleSpriteVertex Vertex;
    // Default value
    Vertex.ParticleId = 0;
    Vertex.OldPosition = {};

    // Essential value
    Vertex.Position = {};
    Vertex.Rotation = 0;
    Vertex.Size = {};
    Vertex.Color = {};
    Vertex.SubImageIndex = 0;
    Vertex.RelativeTime = 0;

    
}

void FTranslucentRenderPass::UpdateBuffers() const
{
    constexpr UINT Stride = sizeof(FParticleSpriteVertex);
    constexpr UINT Offset = 0;
    
    FVertexInfo VertexInfo;
    BufferManager->CreateDynamicVertexBuffer("ParticleVertex", SpriteParticles, VertexInfo);
    BufferManager->UpdateDynamicVertexBuffer("ParticleVertex", SpriteParticles);

    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &VertexInfo.VertexBuffer, &Stride, &Offset);
}

void FTranslucentRenderPass::BindShaders() const
{
    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(L"CascadeParticle");
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(L"CascadeParticle");
    
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
}

void FTranslucentRenderPass::BindTextures()
{
    
}

void FTranslucentRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->RSSetViewports(1, &Viewport->GetViewportResource()->GetD3DViewport());

    constexpr EResourceType ResourceType = EResourceType::ERT_Scene;
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    const FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(ResourceType);
    const FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(ResourceType);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);

    Graphics->DeviceContext->OMSetBlendState(Graphics->BlendState_AlphaBlend, nullptr, 0xffffffff);
    Graphics->DeviceContext->OMSetDepthStencilState(Graphics->DepthStencilState_DepthWriteDisabled, 1);
}

void FTranslucentRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}
