
#include "ShaderRegisters.hlsl"

struct FMeshParticleInstanceVertex
{
    float4 Color;
    float3x4 InstanceToWorld;
    float4 Velocity;
    int16 SubUVParams[4];
    float SubUVLerp;
    float RelativeTime;
};

StructuredBuffer<FMeshParticleInstanceVertex> InstanceBuffer : register(t0);

struct VS_Input 
{
    uint VertexID : SV_VertexID;
    uint InstanceID : SV_InstanceID;
};

struct PS_Input
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
    float RelativeTime : TEXCOORD1;
    float ParticleId : TEXCOORD2;
    float SubImageIndex : TEXCOORD3;
};

PS_Input main(VS_Input Input)
{
    
}
