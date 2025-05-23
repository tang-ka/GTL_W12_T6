Texture2D FogTexture : register(t103);
Texture2D DoFTexture : register(t106);
// PostProcessing 추가 시 Texture 추가 (EShaderSRVSlot)

SamplerState CompositingSampler : register(s0);

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_Input mainVS(uint VertexID : SV_VertexID)
{
    PS_Input Output;

    float2 QuadPositions[6] = {
        float2(-1,  1),  // Top Left
        float2(1,  1),  // Top Right
        float2(-1, -1),  // Bottom Left
        float2(1,  1),  // Top Right
        float2(1, -1),  // Bottom Right
        float2(-1, -1)   // Bottom Left
    };

    float2 UVs[6] = {
        float2(0, 0), float2(1, 0), float2(0, 1),
        float2(1, 0), float2(1, 1), float2(0, 1)
    };

    Output.Position = float4(QuadPositions[VertexID], 0, 1);
    Output.UV = UVs[VertexID];

    return Output;
}

float4 mainPS(PS_Input input) : SV_Target
{
    float2 UV = input.UV;
    float4 FogColor = FogTexture.Sample(CompositingSampler, UV);
    float4 DoFColor = DoFTexture.Sample(CompositingSampler, UV);
    
    // Fog 효과가 적용된 씬에 DoF 효과 적용
    // DoF의 알파가 블러 강도를 나타냄 - 더 자연스러운 합성을 위한 lerp 사용
    float4 FinalColor = lerp(FogColor, DoFColor, DoFColor.a);
    
    return FinalColor;
}
