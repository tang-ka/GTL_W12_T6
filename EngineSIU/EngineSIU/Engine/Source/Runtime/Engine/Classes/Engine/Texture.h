#pragma once
#include "D3D11RHI/GraphicDevice.h"


struct FTexture
{
    FTexture(ID3D11ShaderResourceView* SRV, ID3D11Texture2D* Texture2D, ESamplerType InSamplerType, const FWString& InName, uint32 NewWidth, uint32 NewHeight)
        : Name(InName), TextureSRV(SRV), Texture(Texture2D), SamplerType(InSamplerType), Width(NewWidth), Height(NewHeight)
    {}
    ~FTexture() = default;

    void Release()
    {
        if (TextureSRV)
        {
            TextureSRV->Release();
            TextureSRV = nullptr;
        }
        if (Texture)
        {
            Texture->Release();
            Texture = nullptr;
        }
    }
    
    FWString Name;
    
    ID3D11ShaderResourceView* TextureSRV = nullptr;
    ID3D11Texture2D* Texture = nullptr;
    
    ESamplerType SamplerType = ESamplerType::Linear;
    
    uint32 Width;
    uint32 Height;
};
