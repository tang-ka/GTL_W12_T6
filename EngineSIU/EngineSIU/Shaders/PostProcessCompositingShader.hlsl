Texture2D FogTexture : register(t103);
Texture2D DoFTexture : register(t106); // Depth of Field texture
Texture2D SceneTexture : register(t100); // Original scene texture
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
    
    // Sample all textures
    float4 SceneColor = SceneTexture.Sample(CompositingSampler, UV);
    float4 FogColor = FogTexture.Sample(CompositingSampler, UV);
    float4 DoFColor = DoFTexture.Sample(CompositingSampler, UV);
    
    // Start with the original scene color
    float4 FinalColor = SceneColor;
    
    // Apply fog (fog texture has the scene already blended in it)
    // Use the fog texture directly to avoid black fog issues
    FinalColor = FogColor;
    
    // Apply DoF on top if it has alpha
    if (DoFColor.a > 0.01f)
    {
        // Use pre-multiplied alpha blending for DoF
        // This will overlay the DoF effect on the fog+scene
        float4 BlendedDoF = float4(DoFColor.rgb, 1.0) * DoFColor.a;
        FinalColor = BlendedDoF + FinalColor * (1.0 - DoFColor.a);
    }
    
    return FinalColor;
}
