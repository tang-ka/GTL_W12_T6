
#include "ShaderRegisters.hlsl"

StructuredBuffer<float4x4> BoneMatrices : register(t0);

PS_INPUT_SkeletalMesh mainVS(VS_INPUT_SkeletalMesh Input)
{
    PS_INPUT_SkeletalMesh Output;

    float4 SkinnedPosition = float4(0, 0, 0, 0);
    float3 SkinnedNormal = float3(0, 0, 0);

    for (int i = 0; i < 4; ++i)
    {
        float Weight = Input.BoneWeights[i];
        if (Weight > 0)
        {
            uint BoneIdx = Input.BoneIndices[i];
            float4x4 BoneMatrix = BoneMatrices[BoneIdx];
            
            SkinnedPosition += Weight * mul(float4(Input.Position, 1.0), BoneMatrix);
            SkinnedNormal += Weight * mul(float4(Input.Normal, 0.0), BoneMatrix).xyz;
        }
    }
    
    Output.Position = SkinnedPosition;
    Output.Position = mul(Output.Position, WorldMatrix);
    Output.WorldPosition = Output.Position.xyz;
    
    Output.Position = mul(Output.Position, ViewMatrix);
    Output.Position = mul(Output.Position, ProjectionMatrix);
    
    Output.WorldNormal = mul(SkinnedNormal, (float3x3)InverseTransposedWorld);

    // Begin Tangent
    float3 WorldTangent = mul(Input.Tangent.xyz, (float3x3)WorldMatrix);
    WorldTangent = normalize(WorldTangent);
    WorldTangent = normalize(WorldTangent - Output.WorldNormal * dot(Output.WorldNormal, WorldTangent));

    Output.WorldTangent = float4(WorldTangent, Input.Tangent.w);
    // End Tangent

    Output.Color = Input.Color;
    Output.UV = Input.UV;

    return Output;
}
