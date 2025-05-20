#include "ShaderRegisters.hlsl"

Texture2D ParticleTexture : register(t0); // Particle Texture
SamplerState ParticleSampler : register(s0); // Texture Sampler

const static float2 QuadPos[6] =
{
    float2(-1, -1), float2(1, -1), float2(-1, 1), // 좌하단, 좌상단, 우하단
    float2(-1, 1), float2(1, -1), float2(1, 1) // 좌상단, 우상단, 우하단
};

const static float2 QuadTexCoord[6] =
{
    float2(0, 1), float2(1, 1), float2(0, 0), // 삼각형 1: 좌하단, 우하단, 좌상단
    float2(0, 0), float2(1, 1), float2(1, 0) // 삼각형 2: 좌상단, 우하단, 우상단
};

struct VS_INPUT
{
    uint VertexId        : SV_VertexID;
    float3 Position      : POSITION0;     // FVector Position
    float  RelativeTime  : TEXCOORD0;     // float RelativeTime
    float3 OldPosition   : TEXCOORD1;     // FVector OldPosition
    float2 Size          : TEXCOORD3;     // FVector2D Size
    float  Rotation      : TEXCOORD4;     // float Rotation
    float  SubImageIndex : TEXCOORD5;     // float SubImageIndex
    float4 Color         : COLOR0;        // FLinearColor Color
};

struct PS_INPUT
{
    float4 PositionPS    : SV_POSITION;   // 클립 공간에서의 정점 위치 (필수)
    float4 Color         : COLOR0;        // 파티클 색상
    float2 TextureUV     : TEXCOORD0;     // 텍스처 UV 좌표
    float  RelativeTime  : TEXCOORD1;     // 상대 시간 (픽셀 셰이더에서 효과에 사용 가능)
    float  SubImageIndex : TEXCOORD2;     // SubImage 인덱스 (픽셀 셰이더에서 UV조작 등에 사용 가능)
};


PS_INPUT mainVS(VS_INPUT Input)
{
    PS_INPUT Output = (PS_INPUT)0;
    
    // 카메라를 향하는 billboard 좌표계 생성
    float3 forward = normalize(ViewWorldLocation - Input.Position);
    float3 up = float3(0, 0, 1);
    float3 right = normalize(cross(up, forward));
    up = cross(forward, right);

    // 쿼드 정점 계산 (위치 기준으로 offset)
    float2 offset = QuadPos[Input.VertexId];
    float3 worldPos = Input.Position + offset.x * right * Input.Size.x + offset.y * up * Input.Size.y;

    // 변환
    float4 viewPos = mul(float4(worldPos, 1.0), ViewMatrix);
    
    Output.PositionPS = mul(viewPos, ProjectionMatrix);
    Output.TextureUV = QuadTexCoord[Input.VertexId];
    
    Output.Color = Input.Color;
    Output.RelativeTime = Input.RelativeTime;
    Output.SubImageIndex = Input.SubImageIndex;
    
    return Output;
}

float4 mainPS(PS_INPUT Input)
{
    float4 textureColor = ParticleTexture.Sample(ParticleSampler, Input.TextureUV);
    float4 finalColor = textureColor * Input.Color;

    // 알파가 0이면 그리지 않음 (불필요한 연산 방지 및 완전 투명 픽셀 discard)
    // 알파 테스트가 필요한 경우 활성화
    if (finalColor.a < 0.01f)
    {
        discard;
    }

    return finalColor;
}
