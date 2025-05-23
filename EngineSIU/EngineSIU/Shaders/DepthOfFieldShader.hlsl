// Scene texture and depth texture
Texture2D SceneTexture : register(t100);
Texture2D SceneDepthTexture : register(t99);

// DoF constant buffer
cbuffer DepthOfFieldConstants : register(b0)
{
    float FocusDepth;
    float FocusRange;
    float MaxBlurAmount;
    float Padding;
}

SamplerState DoFSampler : register(s0);

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_Input mainVS(uint VertexID : SV_VertexID)
{
    PS_Input Output;

    float2 QuadPositions[6] =
    {
        float2(-1, 1), // Top Left
        float2(1, 1),  // Top Right
        float2(-1, -1), // Bottom Left
        float2(1, 1),  // Top Right
        float2(1, -1), // Bottom Right
        float2(-1, -1) // Bottom Left
    };

    float2 UVs[6] =
    {
        float2(0, 0), float2(1, 0), float2(0, 1),
        float2(1, 0), float2(1, 1), float2(0, 1)
    };

    Output.Position = float4(QuadPositions[VertexID], 0, 1);
    Output.UV = UVs[VertexID];

    return Output;
}

float4 mainPS(PS_Input Input) : SV_Target
{
    float2 UV = Input.UV;
    
    float4 SceneColor = SceneTexture.Sample(DoFSampler, UV);
    float PixelDepth = SceneDepthTexture.Sample(DoFSampler, UV).r;
    
    // Calculate blur amount using the formula provided in the task
    float BlurAmount = saturate(abs(PixelDepth - FocusDepth) / FocusRange);
    
    // Apply blur effect
    if (BlurAmount > 0.01f)
    {
        float4 BlurredColor = float4(0, 0, 0, 0);
        const int BlurRadius = 5;        // Much larger radius for extreme blur effect
        const float BlurStep = 0.001f;    // Much larger step size for dramatic blur effect
        int SampleCount = 0;
        
        // @todo 블러 로직 개선 ex) Gaussian blur or other techniques
        // Basic box blur based on blur amount
        for (int x = -BlurRadius; x <= BlurRadius; x++)
        {
            for (int y = -BlurRadius; y <= BlurRadius; y++)
            {
                // Skip if too far from center
                if (x*x + y*y > BlurRadius*BlurRadius)
                    continue;
                    
                float2 Offset = float2(x, y) * BlurAmount * BlurStep;
                BlurredColor += SceneTexture.Sample(DoFSampler, UV + Offset);
                SampleCount++;
            }
        }
        
        if (SampleCount > 0)
            BlurredColor /= SampleCount;
            
        return float4(BlurredColor.rgb, BlurAmount); 
    }
    
    return float4(SceneColor.rgb, 0.0);
}
