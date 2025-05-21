
#include "ShaderRegisters.hlsl"

cbuffer MaterialConstants : register(b0)
{
    FMaterial Material;
}

cbuffer SubUVConstant : register(b1)
{
    float2 UVOffset;
    float2 UVScale; // sub UV 셀의 크기 (예: 1/CellsPerColumn, 1/CellsPerRow)
}

struct PS_Input
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
    float RelativeTime : TEXCOORD1;
    float ParticleId : TEXCOORD2;
    float SubImageIndex : TEXCOORD3;
};

float4 main(PS_Input Input) : SV_TARGET
{
    float4 FinalColor = float4(0.f, 0.f, 0.f, 1.f);
    
    float2 UV = Input.UV * UVScale + UVOffset;

    float4 Color = Input.Color;
    if (Material.TextureFlag & TEXTURE_FLAG_DIFFUSE)
    {
        Color *= MaterialTextures[TEXTURE_SLOT_DIFFUSE].Sample(MaterialSamplers[TEXTURE_SLOT_DIFFUSE], UV);        
    }

    FinalColor = Color;
    
    return FinalColor;
}
