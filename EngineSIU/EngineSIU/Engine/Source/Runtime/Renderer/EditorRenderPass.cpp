#include "EditorRenderPass.h"

#include "EngineLoop.h" // GEngineLoop
#include "Engine/Source/Runtime/Engine/Classes/Engine/Engine.h" // GEngine
#include "Engine/Source/Runtime/CoreUObject/UObject/Casts.h"
#include "Engine/Source/Runtime/Engine/Classes/Engine/EditorEngine.h"
#include <D3D11RHI/DXDShaderManager.h>

#include "UnrealClient.h"
#include "Engine/Source/Runtime/Engine/World/World.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Engine/FObjLoader.h"
#include "Engine/Classes/Actors/Player.h"
#include "Engine/Classes/Components/Light/LightComponent.h"
#include "Engine/Classes/Components/Light/DirectionalLightComponent.h"
#include "Engine/Classes/Components/Light/SpotLightComponent.h"
#include "Engine/Classes/Components/Light/PointLightComponent.h"
#include "Engine/Classes/Components/HeightFogComponent.h"
#include "PropertyEditor/ShowFlags.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "Engine/Asset/PhysicsAsset.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

void FEditorRenderPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManager)
{
    FRenderPassBase::Initialize(InBufferManager, InGraphics, InShaderManager);
    
    CreateShaders();
    CreateBuffers();
    CreateConstantBuffers();
}

void FEditorRenderPass::CreateShaders()
{
    D3D11_INPUT_ELEMENT_DESC LayoutGizmo[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11_INPUT_ELEMENT_DESC LayoutPosOnly[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    auto AddShaderSet = [this](const std::wstring& KeyPrefix, const std::string& VsEntry, const std::string& PsEntry, const D3D11_INPUT_ELEMENT_DESC* Layout, uint32_t LayoutSize)
        {
            ShaderManager->AddVertexShaderAndInputLayout(KeyPrefix + L"VS", L"Shaders/EditorShader.hlsl", VsEntry, Layout, LayoutSize);
            ShaderManager->AddPixelShader(KeyPrefix + L"PS", L"Shaders/EditorShader.hlsl", PsEntry);
        };

    auto AddShaderSetWithoutLayout = [this](const std::wstring& KeyPrefix, const std::string& VsEntry, const std::string& PsEntry)
    {
        ShaderManager->AddVertexShader(KeyPrefix + L"VS", L"Shaders/EditorShader.hlsl", VsEntry);
        ShaderManager->AddPixelShader(KeyPrefix + L"PS", L"Shaders/EditorShader.hlsl", PsEntry);
    };
    

    // Cone
    AddShaderSetWithoutLayout(L"Cone", "ConeVS", "ConePS");

    // Icons
    AddShaderSetWithoutLayout(L"Icon", "IconVS", "IconPS");
    
    // Arrow (기즈모 layout 재사용)
    AddShaderSet(L"Arrow", "ArrowVS", "ArrowPS", LayoutGizmo, ARRAYSIZE(LayoutGizmo));
    
    AddShaderSet(L"Sphere", "SphereVS", "SpherePS", LayoutPosOnly, ARRAYSIZE(LayoutPosOnly));
    AddShaderSet(L"Box", "BoxVS", "BoxPS", LayoutPosOnly, ARRAYSIZE(LayoutPosOnly));
    AddShaderSet(L"Capsule", "CapsuleVS", "CapsulePS", LayoutPosOnly, ARRAYSIZE(LayoutPosOnly));
}

void FEditorRenderPass::CreateBuffers()
{
    FVertexInfo OutVertexInfo;
    FIndexInfo OutIndexInfo;
    
    ////////////////////////////////////
    // Box 버퍼 생성
    const TArray<FVector> CubeFrameVertices = {
        { -1.f, -1.f, -1.f }, // 0
        { -1.f, 1.f, -1.f }, // 1
        { 1.f, -1.f, -1.f }, // 2
        { 1.f, 1.f, -1.f }, // 3
        { -1.f, -1.f, 1.f }, // 4
        { 1.f, -1.f, 1.f }, // 5
        { -1.f, 1.f, 1.f }, // 6
        { 1.f, 1.f, 1.f }, // 7
    };

    const TArray<uint32> CubeFrameIndices = {
        //// Bottom face
        //0, 1, 1, 3, 3, 2, 2, 0,
        //// Top face
        //4, 6, 6, 7, 7, 5, 5, 4,
        //// Side faces
        //0, 4, 1, 6, 2, 5, 3, 7
        // 앞면 (Front face) - z = -1 면
        0, 1, 3, 0, 3, 2,

        // 뒷면 (Back face) - z = 1 면  
        4, 5, 7, 4, 7, 6,

        // 왼쪽면 (Left face) - x = -1 면
        0, 4, 6, 0, 6, 1,

        // 오른쪽면 (Right face) - x = 1 면
        2, 3, 7, 2, 7, 5,

        // 아래면 (Bottom face) - y = -1 면
        0, 2, 5, 0, 5, 4,

        // 위면 (Top face) - y = 1 면
        1, 6, 7, 1, 7, 3
    };


    BufferManager->CreateVertexBuffer<FVector>(TEXT("CubeVertexBuffer"), CubeFrameVertices, OutVertexInfo, D3D11_USAGE_IMMUTABLE, 0);
    BufferManager->CreateIndexBuffer<uint32>(TEXT("CubeIndexBuffer"), CubeFrameIndices, OutIndexInfo, D3D11_USAGE_IMMUTABLE, 0);

    Resources.Primitives.Box.VertexInfo = OutVertexInfo;
    Resources.Primitives.Box.IndexInfo = OutIndexInfo;
    
    ////////////////////////////////////
    // Sphere 버퍼 생성
    const TArray<FVector> SphereFrameVertices = GenerateUVSphereVertices(16, 32);
    const TArray<uint32> SphereFrameIndices = GenerateUVSphereIndices(16, 32);

    
    BufferManager->CreateVertexBuffer<FVector>(TEXT("SphereVertexBuffer"), SphereFrameVertices, OutVertexInfo, D3D11_USAGE_IMMUTABLE, 0);
    BufferManager->CreateIndexBuffer<uint32>(TEXT("SphereIndexBuffer"), SphereFrameIndices, OutIndexInfo);

    Resources.Primitives.Sphere.VertexInfo = OutVertexInfo;
    Resources.Primitives.Sphere.IndexInfo = OutIndexInfo;
}

void FEditorRenderPass::CreateConstantBuffers()
{
    BufferManager->CreateBufferGeneric<FConstantBufferDebugBox>("BoxConstantBuffer", nullptr, sizeof(FConstantBufferDebugBox) * ConstantBufferSizeBox, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FConstantBufferDebugSphere>("SphereConstantBuffer", nullptr, sizeof(FConstantBufferDebugSphere) * ConstantBufferSizeSphere, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FConstantBufferDebugCapsule>("CapsuleConstantBuffer", nullptr, sizeof(FConstantBufferDebugCapsule) * ConstantBufferSizeCapsule, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FConstantBufferDebugCone>("ConeConstantBuffer", nullptr, sizeof(FConstantBufferDebugCone) * ConstantBufferSizeCone, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FConstantBufferDebugGrid>("GridConstantBuffer", nullptr, sizeof(FConstantBufferDebugGrid), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FConstantBufferDebugIcon>("IconConstantBuffer", nullptr, sizeof(FConstantBufferDebugIcon), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    BufferManager->CreateBufferGeneric<FConstantBufferDebugArrow>("ArrowConstantBuffer", nullptr, sizeof(FConstantBufferDebugArrow) * ConstantBufferSizeArrow, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}

void FEditorRenderPass::BindRenderTarget(const std::shared_ptr<FEditorViewportClient>& Viewport) const
{
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    FRenderTargetRHI* RenderTargetRHI = ViewportResource->GetRenderTarget(EResourceType::ERT_Editor);
    FDepthStencilRHI* DepthStencilRHI = ViewportResource->GetDepthStencil(EResourceType::ERT_Scene);
    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, DepthStencilRHI->DSV);
}

void FEditorRenderPass::BindShaderResource(const std::wstring& VertexKey, const std::wstring& PixelKey, D3D_PRIMITIVE_TOPOLOGY Topology) const
{
    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(VertexKey);
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(PixelKey);
    ID3D11InputLayout* InputLayout = ShaderManager->GetInputLayoutByKey(VertexKey);
    
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(InputLayout);
    Graphics->DeviceContext->IASetPrimitiveTopology(Topology);
}

void FEditorRenderPass::BindBuffers(const FDebugPrimitiveData& InPrimitiveData) const
{
    constexpr UINT Offset = 0;
    Graphics->DeviceContext->IASetVertexBuffers(0, 1, &InPrimitiveData.VertexInfo.VertexBuffer, &InPrimitiveData.VertexInfo.Stride, &Offset);
    Graphics->DeviceContext->IASetIndexBuffer(InPrimitiveData.IndexInfo.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void FEditorRenderPass::PrepareRenderArr()
{
	if (GEngine->ActiveWorld->WorldType != EWorldType::Editor && GEngine->ActiveWorld->WorldType != EWorldType::PhysicsViewer)
	{
		return;
	}
    
    // gizmo 제외하고 넣기
    for (const auto* Actor : TObjectRange<AActor>())
    {
        for (const auto* Component : Actor->GetComponents())
        {
            // AABB용 static mesh component
            if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Component))
            {
                if (!StaticMesh->IsA<UGizmoBaseComponent>())
                {
                    Resources.Components.StaticMeshComponent.Add(StaticMesh);
                }
            }

            // light
            if (ULightComponentBase* Light = Cast<ULightComponentBase>(Component))
            {
                Resources.Components.Light.Add(Light);
            }

            // fog
            if (UHeightFogComponent* Fog = Cast<UHeightFogComponent>(Component))
            {
                Resources.Components.Fog.Add(Fog);
            }

            if (USphereComponent* SphereComponent = Cast<USphereComponent>(Component))
            {
                Resources.Components.SphereComponents.Add(SphereComponent);
            }

            if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(Component))
            {
                Resources.Components.BoxComponents.Add(BoxComponent);
            }
            
            if (UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(Component))
            {
                Resources.Components.CapsuleComponents.Add(CapsuleComponent);
            }
        }
    }
}

void FEditorRenderPass::ClearRenderArr()
{
    Resources.Components.StaticMeshComponent.Empty();
    Resources.Components.Light.Empty();
    Resources.Components.Fog.Empty();
    Resources.Components.SphereComponents.Empty();
    Resources.Components.CapsuleComponents.Empty();
    Resources.Components.BoxComponents.Empty();
}

void FEditorRenderPass::PrepareRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

void FEditorRenderPass::CleanUpRender(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
}

// 꼼수로 이미 로드된 리소스를 사용
// GUObjectArray에 안올라가게 우회
void FEditorRenderPass::LazyLoad()
{
    // Resourcemanager에서 로드된 texture의 포인터를 가져옴
    // FResourceManager::Initialize에 이미 추가되어 있어야 함
    Resources.IconTextures[EIconType::DirectionalLight] = FEngineLoop::ResourceManager.GetTexture(L"Assets/Editor/Icon/DirectionalLight_64x.png");
    Resources.IconTextures[EIconType::PointLight] = FEngineLoop::ResourceManager.GetTexture(L"Assets/Editor/Icon/PointLight_64x.png");
    Resources.IconTextures[EIconType::SpotLight] = FEngineLoop::ResourceManager.GetTexture(L"Assets/Editor/Icon/SpotLight_64x.png");
    Resources.IconTextures[EIconType::AmbientLight] = FEngineLoop::ResourceManager.GetTexture(L"Assets/Editor/Icon/AmbientLight_64x.png");
    Resources.IconTextures[EIconType::ExponentialFog] = FEngineLoop::ResourceManager.GetTexture(L"Assets/Editor/Icon/ExponentialHeightFog_64.png");
    Resources.IconTextures[EIconType::AtmosphericFog] = FEngineLoop::ResourceManager.GetTexture(L"Assets/Editor/Icon/AtmosphericFog_64.png");

    // Gizmo arrow 로드
    FStaticMeshRenderData* RenderData = FObjManager::GetStaticMesh(L"Assets/Editor/Gizmo/GizmoTranslationZ.obj")->GetRenderData();

    FVertexInfo VertexInfo;
    BufferManager->CreateVertexBuffer(RenderData->ObjectName, RenderData->Vertices, VertexInfo);

    FIndexInfo IndexInfo;
    BufferManager->CreateIndexBuffer(RenderData->ObjectName, RenderData->Indices, IndexInfo);
    
    Resources.Primitives.Arrow.VertexInfo.VertexBuffer = VertexInfo.VertexBuffer;
    Resources.Primitives.Arrow.VertexInfo.NumVertices = VertexInfo.NumVertices;
    Resources.Primitives.Arrow.VertexInfo.Stride = sizeof(FStaticMeshVertex); // Directional Light의 Arrow에 해당됨
    Resources.Primitives.Arrow.IndexInfo.IndexBuffer = IndexInfo.IndexBuffer;
    Resources.Primitives.Arrow.IndexInfo.NumIndices = IndexInfo.NumIndices;
}

void FEditorRenderPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    static bool IsLoaded = false;
    if (!IsLoaded)
    {
        LazyLoad();
        IsLoaded = true;
    }

    const uint64 ShowFlag = Viewport->GetShowFlag();

    BindRenderTarget(Viewport);    

    if (ShowFlag & EEngineShowFlags::SF_LightWireframe)
    {
        RenderPointlightInstanced(ShowFlag);
        RenderSpotlightInstanced(ShowFlag);
    }

    if (ShowFlag & EEngineShowFlags::SF_Collision)
    {
        RenderBoxInstanced(ShowFlag);
        RenderSphereInstanced(ShowFlag);
        RenderCapsuleInstanced(ShowFlag);

        RenderPhysicsAssetsDebug(ShowFlag);
    }

    RenderArrowInstanced();
    //RenderIcons(World, ActiveViewport); // 기존 렌더패스에서 아이콘 렌더하고 있으므로 제거

    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    ID3D11Buffer* NullBuffer[1] = { nullptr };
    Graphics->DeviceContext->VSSetConstantBuffers(11, 1, NullBuffer);
    Graphics->DeviceContext->PSSetConstantBuffers(11, 1, NullBuffer);
}

void FEditorRenderPass::RenderPointlightInstanced(uint64 ShowFlag)
{
    BindShaderResource(L"SphereVS", L"SpherePS", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    BindBuffers(Resources.Primitives.Sphere);

    // 위치랑 bounding box 크기 정보 가져오기
    TArray<FConstantBufferDebugSphere> BufferAll;
    for (ULightComponentBase* LightComp : Resources.Components.Light)
    {
        if (UPointLightComponent* PointLightComp = Cast<UPointLightComponent>(LightComp))
        {
            if (ShowFlag & EEngineShowFlags::SF_LightWireframeSelectedOnly)
            {
                if (Cast<UEditorEngine>(GEngine)->GetSelectedActor())
                {
                    if (Cast<UEditorEngine>(GEngine)->GetSelectedActor()->GetComponents().Contains(PointLightComp))
                    {
                        FConstantBufferDebugSphere b;
                        b.Position = PointLightComp->GetComponentLocation();
                        b.Radius = PointLightComp->GetRadius();
                        BufferAll.Add(b);
                    }
                }
            }
            else
            {
                FConstantBufferDebugSphere b;
                b.Position = PointLightComp->GetComponentLocation();
                b.Radius = PointLightComp->GetRadius();
                BufferAll.Add(b);
            }
        }
    }
    
    BufferManager->BindConstantBuffer("SphereConstantBuffer", 11, EShaderStage::Vertex);
    int BufferIndex = 0;
    for (int i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeSphere) * ConstantBufferSizeSphere; ++i)
    {
        TArray<FConstantBufferDebugSphere> SubBuffer;
        for (int j = 0; j < ConstantBufferSizeSphere; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugSphere>(TEXT("SphereConstantBuffer"), SubBuffer);
            Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Sphere.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
        }
    }
}

void FEditorRenderPass::RenderSpotlightInstanced(uint64 ShowFlag)
{
    BindShaderResource(L"ConeVS", L"ConePS", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // 위치랑 bounding box 크기 정보 가져오기
    TArray<FConstantBufferDebugCone> BufferAll;
    for (ULightComponentBase* LightComp : Resources.Components.Light)
    {
        if (USpotLightComponent* SpotLightComp = Cast<USpotLightComponent>(LightComp))
        {
            if (ShowFlag & EEngineShowFlags::SF_LightWireframeSelectedOnly)
            {
                if (Cast<UEditorEngine>(GEngine)->GetSelectedActor())
                {
                    if (Cast<UEditorEngine>(GEngine)->GetSelectedActor()->GetComponents().Contains(SpotLightComp))
                    {
                        FConstantBufferDebugCone b;
                        b.ApexPosition = SpotLightComp->GetComponentLocation();
                        b.Radius = SpotLightComp->GetRadius();
                        b.Direction = SpotLightComp->GetDirection();
                        // Inner Cone
                        b.Angle = SpotLightComp->GetInnerRad();
                        BufferAll.Add(b);
                        // Outer Cone
                        b.Angle = SpotLightComp->GetOuterRad();
                        BufferAll.Add(b);
                    }
                }
            }
            else
            {
                FConstantBufferDebugCone b;
                b.ApexPosition = SpotLightComp->GetComponentLocation();
                b.Radius = SpotLightComp->GetRadius();
                b.Direction = SpotLightComp->GetDirection();
                // Inner Cone
                b.Angle = SpotLightComp->GetInnerRad();
                BufferAll.Add(b);
                // Outer Cone
                b.Angle = SpotLightComp->GetOuterRad();
                BufferAll.Add(b);
            }
        }
    }


    BufferManager->BindConstantBuffer("ConeConstantBuffer", 11, EShaderStage::Vertex);
    int BufferIndex = 0;
    for (int i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeCone) * ConstantBufferSizeCone; ++i)
    {
        TArray<FConstantBufferDebugCone> SubBuffer;
        for (int j = 0; j < ConstantBufferSizeCone; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugCone>(TEXT("ConeConstantBuffer"), SubBuffer);
            // Only Draw Selected SpotLight's Cone = 2 | Cone: (24 * 2) * 2 + Sphere: (10 * 2) * 2 = 136
            Graphics->DeviceContext->DrawInstanced(136, SubBuffer.Num(), 0, 0);
        }
    }
}

// 사용 안함
void FEditorRenderPass::UpdateTextureIcon(EIconType Type)
{
    Graphics->DeviceContext->PSSetShaderResources(0, 1, &Resources.IconTextures[Type]->TextureSRV);
    
    ID3D11SamplerState* SamplerState = Graphics->GetSamplerState(Resources.IconTextures[Type]->SamplerType);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &SamplerState);
}

void FEditorRenderPass::RenderArrowInstanced()
{
    BindShaderResource(L"ArrowVS", L"ArrowPS", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    BindBuffers(Resources.Primitives.Arrow);

    // 1. Collect Instance Data
    TArray<FConstantBufferDebugArrow> BufferAll;
    for (ULightComponentBase* LightComp : Resources.Components.Light)
    {
        constexpr float ArrowScale = 1.0f;
        if (UDirectionalLightComponent* DLightComp = Cast<UDirectionalLightComponent>(LightComp))
        {
            FConstantBufferDebugArrow Buf;
            Buf.Position = DLightComp->GetComponentLocation();
            Buf.ScaleXYZ = ArrowScale;
            Buf.Direction = DLightComp->GetDirection();
            Buf.ScaleZ = ArrowScale;
            BufferAll.Add(Buf);
        }
        else if (USpotLightComponent* SpotComp = Cast<USpotLightComponent>(LightComp))
        {
            FConstantBufferDebugArrow Buf;
            Buf.Position = SpotComp->GetComponentLocation();
            Buf.ScaleXYZ = ArrowScale;
            Buf.Direction = SpotComp->GetDirection();
            Buf.ScaleZ = ArrowScale;
            BufferAll.Add(Buf);
        }
    }

    BufferManager->BindConstantBuffer("ArrowConstantBuffer", 11, EShaderStage::Vertex);

    int32 BufferIndex = 0;
    for (int i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeArrow) * ConstantBufferSizeArrow; ++i)
    {
        TArray<FConstantBufferDebugArrow> SubBuffer;
        for (int32 j = 0; j < ConstantBufferSizeArrow; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugArrow>(TEXT("ArrowConstantBuffer"), SubBuffer);
            Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Arrow.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
        }
    }
}

void FEditorRenderPass::RenderBoxInstanced(uint64 ShowFlag)
{
    BindShaderResource(L"BoxVS", L"BoxPS", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    BindBuffers(Resources.Primitives.Box);
    
    // 위치랑 bounding box 크기 정보 가져오기
    TArray<FConstantBufferDebugBox> BufferAll;
    for (UBoxComponent* BoxComponent : Resources.Components.BoxComponents)
    {
        if (ShowFlag & EEngineShowFlags::SF_CollisionSelectedOnly)
        {
            AActor* Actor = Cast<UEditorEngine>(GEngine)->GetSelectedActor();
            if (Actor && Actor->GetComponents().Contains(BoxComponent))
            {
                FConstantBufferDebugBox b;
                b.WorldMatrix = BoxComponent->GetWorldMatrix();
                b.Extent = BoxComponent->GetBoxExtent();
                BufferAll.Add(b);
            }
        }
        else
        {
            FConstantBufferDebugBox b;
            b.WorldMatrix = BoxComponent->GetWorldMatrix();
            b.Extent = BoxComponent->GetBoxExtent();
            BufferAll.Add(b);
        }
    }
    
    BufferManager->BindConstantBuffer("BoxConstantBuffer", 11, EShaderStage::Vertex);
    int BufferIndex = 0;
    for (uint32 i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeBox) * ConstantBufferSizeBox; ++i)
    {
        TArray<FConstantBufferDebugBox> SubBuffer;
        for (uint32 j = 0; j < ConstantBufferSizeBox; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugBox>(TEXT("BoxConstantBuffer"), SubBuffer);
            Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Box.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
        }
    }
}

void FEditorRenderPass::RenderSphereInstanced(uint64 ShowFlag)
{
    BindShaderResource(L"SphereVS", L"SpherePS", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    BindBuffers(Resources.Primitives.Sphere);

    // 위치랑 bounding box 크기 정보 가져오기
    TArray<FConstantBufferDebugSphere> BufferAll;
    for (USphereComponent* SphereComponent : Resources.Components.SphereComponents)
    {
        if (ShowFlag & EEngineShowFlags::SF_CollisionSelectedOnly)
        {
            AActor* Actor = Cast<UEditorEngine>(GEngine)->GetSelectedActor();
            if (Actor && Actor->GetComponents().Contains(SphereComponent))
            {
                FConstantBufferDebugSphere b;
                b.Position = SphereComponent->GetComponentLocation();
                b.Radius = SphereComponent->GetRadius();
                BufferAll.Add(b);
            }
        }
        else
        {
            FConstantBufferDebugSphere b;
            b.Position = SphereComponent->GetComponentLocation();
            b.Radius = SphereComponent->GetRadius();
            BufferAll.Add(b);
        }
    }

    BufferManager->BindConstantBuffer("SphereConstantBuffer", 11, EShaderStage::Vertex);
    int BufferIndex = 0;
    for (uint32 i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeSphere) * ConstantBufferSizeSphere; ++i)
    {
        TArray<FConstantBufferDebugSphere> SubBuffer;
        for (uint32 j = 0; j < ConstantBufferSizeSphere; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugSphere>(TEXT("SphereConstantBuffer"), SubBuffer);
            Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Sphere.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
        }
    }
}

void FEditorRenderPass::RenderCapsuleInstanced(uint64 ShowFlag)
{
    BindShaderResource(L"CapsuleVS", L"CapsulePS", D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    
    // 위치랑 bounding box 크기 정보 가져오기
    TArray<FConstantBufferDebugCapsule> BufferAll;
    for (UShapeComponent* ShapeComponent : Resources.Components.CapsuleComponents)
    {
        if (UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(ShapeComponent))
        {
            if (ShowFlag & EEngineShowFlags::SF_CollisionSelectedOnly)
            {
                AActor* Actor = Cast<UEditorEngine>(GEngine)->GetSelectedActor();
                if (Actor && Actor->GetComponents().Contains(CapsuleComponent))
                {
                    FConstantBufferDebugCapsule b;
                    b.WorldMatrix = CapsuleComponent->GetWorldMatrix();
                    b.Height = CapsuleComponent->GetHalfHeight();
                    b.Radius = CapsuleComponent->GetRadius();
                    BufferAll.Add(b);
                }
            }
            else
            {
                FConstantBufferDebugCapsule b;
                b.WorldMatrix = CapsuleComponent->GetWorldMatrix();
                b.Height = CapsuleComponent->GetHalfHeight();
                b.Radius = CapsuleComponent->GetRadius();
                BufferAll.Add(b);
            }
        }
    }
    
    BufferManager->BindConstantBuffer("CapsuleConstantBuffer", 11, EShaderStage::Vertex);
    int BufferIndex = 0;
    for (int i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeCapsule) * ConstantBufferSizeCapsule; ++i)
    {
        TArray<FConstantBufferDebugCapsule> SubBuffer;
        for (int j = 0; j < ConstantBufferSizeCapsule; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }
    
        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugCapsule>(TEXT("CapsuleConstantBuffer"), SubBuffer);
            //Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Capsule.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);

            // 수평 링 : stacks + 1개, 수직 줄 stacks 개
            Graphics->DeviceContext->DrawInstanced(1184, SubBuffer.Num(), 0, 0);
        }
    }
}

void FEditorRenderPass::RenderPhysicsAssetDebug(UPhysicsAsset* PhysicsAsset, USkeletalMeshComponent* SkelMeshComp)
{
    if (!PhysicsAsset)
    {
        return;
    }

    Graphics->DeviceContext->RSSetState(FEngineLoop::GraphicDevice.RasterizerSolidBack);

    // 각 BodySetup의 AggregateGeom 렌더링
    for (UBodySetup* BodySetup : PhysicsAsset->GetBodySetups())
    {
        if (!BodySetup)
        {
            continue;
        }
        
        const FReferenceSkeleton& Skeleton = SkelMeshComp->GetSkeletalMeshAsset()->GetSkeleton()->GetReferenceSkeleton();
        int32 BoneIndex = Skeleton.RawNameToIndexMap[BodySetup->BoneName];
        int32 ParentIndex = Skeleton.RawRefBoneInfo[BoneIndex].ParentIndex;
        FTransform BoneTransform = Skeleton.GetRawRefBonePose()[BoneIndex];
        
        // 이 부분 어딘가에 캐싱하는게 좋을 듯
        while (ParentIndex != INDEX_NONE)
        {
            FTransform ParentTransform = Skeleton.RawRefBonePose[ParentIndex];
            BoneTransform = ParentTransform * BoneTransform;
            ParentIndex = Skeleton.RawRefBoneInfo[ParentIndex].ParentIndex;
        }

        FTransform BaseTransform = SkelMeshComp->GetWorldTransform();
        FKAggregateGeom* AggGeom = &(BodySetup->AggGeom);

        RenderBoxElements(AggGeom->BoxElems, BoneTransform, BaseTransform);
        RenderSphereElements(AggGeom->SphereElems, BoneTransform, BaseTransform);
        RenderCapsuleElements(AggGeom->SphylElems, BoneTransform, BaseTransform);
    }
}

void FEditorRenderPass::RenderBoxElements(const TArray<FKBoxElem>& BoxElems, const FTransform& BoneTransform, const FTransform& BaseTransform)
{
    if (BoxElems.Num() == 0) return;

    BindShaderResource(L"BoxVS", L"BoxPS", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    BindBuffers(Resources.Primitives.Box);

    TArray<FConstantBufferDebugBox> BufferAll;

    for (const FKBoxElem& BoxElem : BoxElems)
    {
        FTransform LocalTransform(BoxElem.Rotation, BoxElem.Center, BoxElem.Extent);
        FTransform FinalTransform = BaseTransform * BoneTransform * LocalTransform;

        FConstantBufferDebugBox BufferData;
        BufferData.WorldMatrix = FinalTransform.ToMatrixWithScale();
        BufferData.Extent = BoxElem.Extent;
        BufferAll.Add(BufferData);
    }

    RenderInstancedBoxes(BufferAll);
}

void FEditorRenderPass::RenderInstancedBoxes(const TArray<FConstantBufferDebugBox>& BufferAll)
{
    BufferManager->BindConstantBuffer("BoxConstantBuffer", 11, EShaderStage::Vertex);

    int BufferIndex = 0;
    for (uint32 i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeBox) * ConstantBufferSizeBox; ++i)
    {
        TArray<FConstantBufferDebugBox> SubBuffer;
        for (uint32 j = 0; j < ConstantBufferSizeBox; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugBox>(TEXT("BoxConstantBuffer"), SubBuffer);
            Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Box.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
        }
    }
}

void FEditorRenderPass::RenderSphereElements(const TArray<FKSphereElem>& SphereElems, const FTransform& BoneTransform, const FTransform& BaseTransform)
{
    if (SphereElems.Num() == 0) return;

    BindShaderResource(L"SphereVS", L"SpherePS", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    BindBuffers(Resources.Primitives.Sphere);

    TArray<FConstantBufferDebugSphere> BufferAll;

    for (const FKSphereElem& SphereElem : SphereElems)
    {
        FTransform LocalTransform(FRotator::ZeroRotator, SphereElem.Center, FVector(SphereElem.Radius));
        FTransform FinalTransform = BaseTransform * BoneTransform * LocalTransform;

        FConstantBufferDebugSphere BufferData;
        BufferData.Position = FinalTransform.GetTranslation();
        BufferData.Radius = SphereElem.Radius;
        BufferAll.Add(BufferData);
    }

    RenderInstancedSpheres(BufferAll);
}

void FEditorRenderPass::RenderInstancedSpheres(const TArray<FConstantBufferDebugSphere>& BufferAll)
{
    BufferManager->BindConstantBuffer("SphereConstantBuffer", 11, EShaderStage::Vertex);

    int BufferIndex = 0;
    for (uint32 i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeSphere) * ConstantBufferSizeSphere; ++i)
    {
        TArray<FConstantBufferDebugSphere> SubBuffer;
        for (uint32 j = 0; j < ConstantBufferSizeSphere; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugSphere>(TEXT("SphereConstantBuffer"), SubBuffer);
            Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Sphere.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
        }
    }
}

void FEditorRenderPass::RenderCapsuleElements(const TArray<FKSphylElem>& CapsuleElems, const FTransform& BoneTransform, const FTransform& BaseTransform)
{
    if (CapsuleElems.Num() == 0) return;

    BindShaderResource(L"CapsuleVS", L"CapsulePS", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    TArray<FConstantBufferDebugCapsule> BufferAll;

    for (const FKSphylElem& CapsuleElem : CapsuleElems)
    {
        FTransform LocalTransform(CapsuleElem.Rotation, CapsuleElem.Center, FVector::OneVector);
        FTransform FinalTransform = BaseTransform * BoneTransform * LocalTransform;

        FConstantBufferDebugCapsule BufferData;
        BufferData.WorldMatrix = FinalTransform.ToMatrixWithScale();
        BufferData.Height = CapsuleElem.Length;
        BufferData.Radius = CapsuleElem.Radius;
        BufferAll.Add(BufferData);
    }

    RenderInstancedCapsules(BufferAll);
}

void FEditorRenderPass::RenderInstancedCapsules(const TArray<FConstantBufferDebugCapsule>& BufferAll)
{
    BufferManager->BindConstantBuffer("CapsuleConstantBuffer", 11, EShaderStage::Vertex);

    int BufferIndex = 0;
    for (uint32 i = 0; i < (1 + BufferAll.Num() / ConstantBufferSizeCapsule) * ConstantBufferSizeCapsule; ++i)
    {
        TArray<FConstantBufferDebugCapsule> SubBuffer;
        for (uint32 j = 0; j < ConstantBufferSizeCapsule; ++j)
        {
            if (BufferIndex < BufferAll.Num())
            {
                SubBuffer.Add(BufferAll[BufferIndex]);
                ++BufferIndex;
            }
            else
            {
                break;
            }
        }

        if (SubBuffer.Num() > 0)
        {
            BufferManager->UpdateConstantBuffer<FConstantBufferDebugCapsule>(TEXT("CapsuleConstantBuffer"), SubBuffer);
            //Graphics->DeviceContext->DrawIndexedInstanced(Resources.Primitives.Capsule.IndexInfo.NumIndices, SubBuffer.Num(), 0, 0, 0);
            Graphics->DeviceContext->DrawInstanced(1728, SubBuffer.Num(), 0, 0);
        }
    }
}

void FEditorRenderPass::RenderPhysicsAssetsDebug(uint64 ShowFlag)
{
    // [TEMP] PhysicsViewer 월드에서 PhysicsAsset 찾기
    if (GEngine->ActiveWorld->WorldType == EWorldType::PhysicsViewer || GEngine->ActiveWorld->WorldType == EWorldType::Editor)
    {
        for (const auto* Actor : TObjectRange<AActor>())
        {
            if (USkeletalMeshComponent* SkelMeshComp = Actor->GetComponentByClass<USkeletalMeshComponent>())
            {
                if (UPhysicsAsset* PhysicsAsset = SkelMeshComp->GetPhysicsAsset())
                {
                    RenderPhysicsAssetDebug(PhysicsAsset, SkelMeshComp);
                }
            }
            if (UStaticMeshComponent* StaticMeshComp = Actor->GetComponentByClass<UStaticMeshComponent>())
            {
                if (!StaticMeshComp->GetStaticMesh())
                    continue;
                if (UBodySetup* BodySetup = StaticMeshComp->GetStaticMesh()->GetBodySetup())
                {
                    RenderStaticMeshPhysicsDebug(BodySetup, StaticMeshComp);
                }
            }
        }
    }
}

void FEditorRenderPass::RenderStaticMeshPhysicsDebug(UBodySetup* BodySetup, UStaticMeshComponent* StaticMeshComp)
{
    if (!BodySetup || !StaticMeshComp->ShouldSimulatePhysics())
    {
        return;
    }

    Graphics->DeviceContext->RSSetState(FEngineLoop::GraphicDevice.RasterizerSolidBack);

    FTransform StaticMeshTransform = StaticMeshComp->GetComponentTransform();

    FKAggregateGeom* AggGeom = &(BodySetup->AggGeom);

    RenderBoxElements(AggGeom->BoxElems, StaticMeshTransform, FTransform());
    RenderSphereElements(AggGeom->SphereElems, StaticMeshTransform, FTransform());

    TArray<FKSphylElem> CopiedSphyElems;
    for (auto Elem : AggGeom->SphylElems)
    {
        auto TempElem = Elem;
        TempElem.Rotation += FRotator(90, 0, 0);
        CopiedSphyElems.Add(TempElem);
    }
    auto TempStaticMeshTransform = StaticMeshTransform;
    TempStaticMeshTransform.SetScale3D(FVector(StaticMeshTransform.Scale3D.Z, StaticMeshTransform.Scale3D.Y, StaticMeshTransform.Scale3D.X));
    RenderCapsuleElements(CopiedSphyElems, TempStaticMeshTransform, FTransform());
}

TArray<FVector> FEditorRenderPass::GenerateUVSphereVertices(int32 Rings, int32 Sectors)
{
    TArray<FVector> Vertices;

    for (int32 r = 0; r <= Rings; ++r)
    {
        float Phi = PI * r / Rings; // 위도 각도
        float Y = FMath::Cos(Phi);
        float SinPhi = FMath::Sin(Phi);

        for (int32 s = 0; s <= Sectors; ++s)
        {
            float Theta = 2.0f * PI * s / Sectors; // 경도 각도
            float X = SinPhi * FMath::Cos(Theta);
            float Z = SinPhi * FMath::Sin(Theta);

            Vertices.Add(FVector(X, Y, Z)); // Y-up 기준
        }
    }

    return Vertices;
}

TArray<uint32> FEditorRenderPass::GenerateUVSphereIndices(int32 Rings, int32 Sectors)
{
    TArray<uint32> Indices;

    for (int32 r = 0; r < Rings; ++r)
    {
        for (int32 s = 0; s < Sectors; ++s)
        {
            int32 Current = r * (Sectors + 1) + s;
            int32 Next = Current + Sectors + 1;

            // 첫 번째 삼각형
            Indices.Add(Current);
            Indices.Add(Next);
            Indices.Add(Current + 1);

            // 두 번째 삼각형
            Indices.Add(Current + 1);
            Indices.Add(Next);
            Indices.Add(Next + 1);
        }
    }

    return Indices;
}
