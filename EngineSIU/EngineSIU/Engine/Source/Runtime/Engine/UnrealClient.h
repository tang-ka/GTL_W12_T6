#pragma once
#include "Define.h" 
#include <d3d11.h>

#include "Container/Map.h"

class FViewportResource;

enum class EViewScreenLocation : uint8
{
    EVL_TopLeft,
    EVL_TopRight,
    EVL_BottomLeft,
    EVL_BottomRight,
    EVL_MAX,
};

enum class EResourceType : uint8
{
    ERT_Debug,
    ERT_Scene,
    ERT_Editor,
    ERT_Gizmo,
    //ERT_Overlay,
    ERT_PP_Fog,
    ERT_PP_CameraEffect,
    /// Begine DoF
    ERT_DoF_LayerInfo,
    ERT_DoF_LayerNear,
    ERT_DoF_LayerFar,
    ERT_DoF_Compositing,
    /// End Dof
    ERT_PostProcessCompositing,
    ERT_Compositing,
    ERT_Temp1,
    ERT_Temp2,
    ERT_MAX,
};

/**
 * 텍스처의 한 축을 기준으로 길이를 얼마나 줄일지를 결정
 *
 *   e.g. 2x: Down Sampled Width = Original Width / 2;
 *            Down Sampled Height = Original Height / 2;
 *            --> Down Sampled Size = Original Size / (2 * 2);
 */
enum class EDownSampleScale : uint8
{
    DSS_None = 1,
    DSS_2x = 2,
    DSS_4x = 4,
    DSS_8x = 8,
    DSS_MAX = UINT8_MAX,
};

struct FRenderTargetRHI
{
    ID3D11Texture2D* Texture2D = nullptr;
    ID3D11RenderTargetView* RTV = nullptr;
    ID3D11ShaderResourceView* SRV = nullptr;
     
    void Release()
    {
        if (SRV)
        {
            SRV->Release();
            SRV = nullptr;
        }
        if (RTV)
        {
            RTV->Release();
            RTV = nullptr;
        }
        if (Texture2D)
        {
            Texture2D->Release();
            Texture2D = nullptr;
        }
    }
};

struct FDepthStencilRHI
{
    ID3D11Texture2D* Texture2D = nullptr;
    ID3D11DepthStencilView* DSV = nullptr;
    ID3D11ShaderResourceView* SRV = nullptr;

    void Release()
    {
        if (SRV)
        {
            SRV->Release();
            SRV = nullptr;
        }
        if (DSV)
        {
            DSV->Release();
            DSV = nullptr;
        }
        if (Texture2D)
        {
            Texture2D->Release();
            Texture2D = nullptr;
        }
    }
};

class FViewportResource
{
public:
    FViewportResource();
    ~FViewportResource();

    void Initialize(uint32 InWidth, uint32 InHeight);
    void Resize(uint32 NewWidth, uint32 NewHeight);

    void Release();
    
    D3D11_VIEWPORT& GetD3DViewport() { return D3DViewport; }

    ////////
    /// Depth Stencil
    ////////
    HRESULT CreateDepthStencil(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None);

    // 해당 타입의 리소스를 리턴. 없는 경우에는 생성해서 리턴.
    FDepthStencilRHI* GetDepthStencil(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None);

    bool HasDepthStencil(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None) const;

    // 가지고있는 모든 리소스의 렌더 타겟 뷰를 clear
    void ClearDepthStencils(ID3D11DeviceContext* DeviceContext);

    // 지정한 타입의 렌더 타겟 뷰를 clear
    void ClearDepthStencil(ID3D11DeviceContext* DeviceContext, EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None);

    ////////
    /// Render Target
    ////////
    HRESULT CreateRenderTarget(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None);

    // 해당 타입의 리소스를 리턴. 없는 경우에는 생성해서 리턴.
    FRenderTargetRHI* GetRenderTarget(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None);

    bool HasRenderTarget(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None) const;

    // 가지고있는 모든 리소스의 렌더 타겟 뷰를 clear
    void ClearRenderTargets(ID3D11DeviceContext* DeviceContext);

    // 지정한 타입의 렌더 타겟 뷰를 clear
    void ClearRenderTarget(ID3D11DeviceContext* DeviceContext, EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_None);

    ////////
    /// ClearColor
    ////////
    std::array<float, 4> GetClearColor(EResourceType Type) const;
    
private:
    // DirectX
    D3D11_VIEWPORT D3DViewport = {};

    TMap<EResourceType, TMap<EDownSampleScale, FDepthStencilRHI>> DepthStencils;
    TMap<EResourceType, TMap<EDownSampleScale, FRenderTargetRHI>> RenderTargets;

    void ReleaseAllResources();
    void ReleaseDepthStencil(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_MAX);
    void ReleaseRenderTarget(EResourceType Type, EDownSampleScale DownSampleScale = EDownSampleScale::DSS_MAX);

    /**
     * ClearColors 맵에는 모든 EResourceType에 대응하는 색상을
     * 이 클래스의 생성자에서 반드시 추가해야 함.
     */
    TMap<EResourceType, std::array<float, 4>> ClearColors;
};


class FViewport
{
public:
    FViewport();
    FViewport(EViewScreenLocation InViewLocation);
    ~FViewport();

    void Initialize(const FRect& InRect);
    void ResizeViewport(const FRect& InRect);
    void ResizeViewport(const FRect& Top, const FRect& Bottom, const FRect& Left, const FRect& Right);

    D3D11_VIEWPORT& GetD3DViewport() const { return ViewportResource->GetD3DViewport(); }

    EViewScreenLocation GetViewLocation() const { return ViewLocation; }

    FViewportResource* GetViewportResource() const { return ViewportResource; }

    FRect GetRect() const { return Rect; }

    bool bIsHovered(const FVector2D& InPoint) const;

private:
    FViewportResource* ViewportResource;

    EViewScreenLocation ViewLocation;   // 뷰포트 위치

    // 이 값은 화면의 크기 뿐만 아니라 위치 정보도 가지고 있음.
    FRect Rect;
};
