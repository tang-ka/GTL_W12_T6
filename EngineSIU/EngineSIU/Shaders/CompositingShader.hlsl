Texture2D SceneTexture : register(t100);
Texture2D TranslucentTexture : register(t101);
Texture2D PP_PostProcessTexture : register(t103);
Texture2D EditorTexture : register(t104);
Texture2D EditorOverlayTextyre : register(t105);
Texture2D DebugTexture : register(t110);
Texture2D CameraEffectTexture : register(t111);

SamplerState CompositingSampler : register(s0);

// ShowFlag 정의 (ShowFlags.h와 동일하게 유지)
#define SF_DepthOfFieldLayer (1U << 13)
#define SF_LightHeatMap (1U << 14)

cbuffer ShowFlagBuffer : register(b0)
{
    uint ShowFlags;
    uint3 ShowFlagPadding;
}

cbuffer Gamma : register(b1)
{
    float GammaValue;
    float3 GammaPadding;
}

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

float4 mainPS(PS_Input Input) : SV_TARGET
{
    float4 Scene = SceneTexture.Sample(CompositingSampler, Input.UV);
    Scene = pow(Scene, GammaValue);
    float4 Translucent = TranslucentTexture.Sample(CompositingSampler, Input.UV);
    float4 PostProcess = PP_PostProcessTexture.Sample(CompositingSampler, Input.UV);
    float4 Editor = EditorTexture.Sample(CompositingSampler, Input.UV);
    float4 EditorOverlay = EditorOverlayTextyre.Sample(CompositingSampler, Input.UV);
    float4 Debug = DebugTexture.Sample(CompositingSampler, Input.UV);
    float4 CameraEffect = CameraEffectTexture.Sample(CompositingSampler, Input.UV);
    
    float4 FinalColor = Scene;

    FinalColor = lerp(FinalColor, PostProcess, PostProcess.a);
    FinalColor = lerp(FinalColor, Editor, Editor.a);
    FinalColor = lerp(FinalColor, Translucent, Translucent.a);
    FinalColor = lerp(FinalColor, EditorOverlay, EditorOverlay.a);
    FinalColor = lerp(FinalColor, CameraEffect, CameraEffect.a);

    if (ShowFlags & SF_LightHeatMap)
    {
        FinalColor = lerp(FinalColor, Debug, 0.5);
    }
    if (ShowFlags & SF_DepthOfFieldLayer)
    {
        // Skip if Focal Distance is 0.0f
        if(Debug.b != 1.0f)
        {
            FinalColor = lerp(FinalColor, Debug.bgra, 0.5);
        }
    }
    
    return FinalColor;
}
