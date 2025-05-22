
#include "ShaderRegisters.hlsl"

struct FMeshParticleInstanceVertex
{
    float4 Color;
    matrix TransformMatrix;
};

StructuredBuffer<FMeshParticleInstanceVertex> InstanceBuffer : register(t1);

PS_INPUT_CommonMesh main(VS_INPUT_StaticMesh Input, uint InstanceID : SV_InstanceID)
{
    FMeshParticleInstanceVertex Instance = InstanceBuffer[InstanceID];
    
    PS_INPUT_CommonMesh Output;
    Output.Position = float4(Input.Position, 1.0);
    Output.Position = mul(Output.Position, Instance.TransformMatrix);
    Output.WorldPosition = Output.Position.xyz;

    Output.Position = mul(Output.Position, WorldMatrix);
    Output.Position = mul(Output.Position, ViewMatrix);
    Output.Position = mul(Output.Position, ProjectionMatrix);

    Output.WorldNormal = normalize(mul(Input.Normal, (float3x3)InverseTransposedWorld));

    // Begin Tangent
    float3 WorldTangent = mul(Input.Tangent.xyz, (float3x3)WorldMatrix);
    WorldTangent = normalize(WorldTangent);
    WorldTangent = normalize(WorldTangent - Output.WorldNormal * dot(Output.WorldNormal, WorldTangent));

    Output.WorldTangent = float4(WorldTangent, Input.Tangent.w);
    
    Output.UV = Input.UV;
    Output.Color = Instance.Color;

    return Output;
}
