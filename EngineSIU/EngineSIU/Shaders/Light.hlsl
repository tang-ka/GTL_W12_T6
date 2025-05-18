
#define MAX_LIGHTS 16 

#define MAX_DIRECTIONAL_LIGHT 16
#define MAX_POINT_LIGHT 16
#define MAX_SPOT_LIGHT 16
#define MAX_AMBIENT_LIGHT 16

#define POINT_LIGHT         1
#define SPOT_LIGHT          2
#define DIRECTIONAL_LIGHT   3
#define AMBIENT_LIGHT       4

#define MAX_LIGHT_PER_TILE 1024

#define NEAR_PLANE 0.1
#define LIGHT_RADIUS_WORLD 7

#define MAX_CASCADE_NUM 5

struct FAmbientLightInfo
{
    float4 AmbientColor;
};

struct FDirectionalLightInfo
{
    float4 LightColor;

    float3 Direction;
    float Intensity;
    
    row_major matrix LightViewProj;
    row_major matrix LightInvProj; // 섀도우맵 생성 시 사용한 VP 행렬
    
    uint ShadowMapArrayIndex;//캐스캐이드전 임시 배열
    uint  CastShadows;
    float ShadowBias;
    float Padding3; // 필요시

    float OrthoWidth;
    // 직교 투영 볼륨의 월드 단위 높이 (섀도우 영역)
    float OrthoHeight;
    // 섀도우 계산을 위한 라이트 시점의 Near Plane (음수 가능)
    float ShadowNearPlane;
    // 섀도우 계산을 위한 라이트 시점의 Far Plane
    float ShadowFarPlane;
};

struct FPointLightInfo
{
    float4 LightColor;

    float3 Position;
    float Radius;

    int Type;
    float Intensity;
    float Attenuation;
    float Padding;

    // --- Shadow Info ---
    row_major matrix LightViewProj[6]; // 섀도우맵 생성 시 사용한 VP 행렬
    
    uint CastShadows;
    float ShadowBias;
    uint ShadowMapArrayIndex; // 필요시
    float Padding2; // 필요시
};

struct FSpotLightInfo
{
    float4 LightColor;

    float3 Position;
    float Radius;

    float3 Direction;
    float Intensity;

    int Type;
    float InnerRad;
    float OuterRad;
    float Attenuation;
    
    // --- Shadow Info ---
    row_major matrix LightViewProj; // 섀도우맵 생성 시 사용한 VP 행렬
    
    uint CastShadows;
    float ShadowBias;
    uint ShadowMapArrayIndex; // 필요시
    float Padding2; // 필요시
};

cbuffer FLightInfoBuffer : register(b0)
{
    FAmbientLightInfo Ambient[MAX_AMBIENT_LIGHT];
    FDirectionalLightInfo Directional[MAX_DIRECTIONAL_LIGHT];
    FPointLightInfo PointLights[MAX_POINT_LIGHT]; //삭제 예정
    FSpotLightInfo SpotLights[MAX_SPOT_LIGHT]; // 삭제 예정
    
    int DirectionalLightsCount;
    int PointLightsCount;
    int SpotLightsCount;
    int AmbientLightsCount;
};

cbuffer ShadowFlagConstants : register(b5)
{
    uint IsShadow;
    float3 shadowFlagPad0;
}

cbuffer CascadeConstantBuffer : register(b9)
{
    row_major matrix World;
    row_major matrix CascadedViewProj[MAX_CASCADE_NUM];
    row_major matrix CascadedInvViewProj[MAX_CASCADE_NUM];
    row_major matrix CascadedInvProj[MAX_CASCADE_NUM];
    float4 CascadeSplits;
    float2 cascadepad;
};

struct LightPerTiles
{
    uint NumLights;
    uint Indices[MAX_LIGHT_PER_TILE];
    uint Padding[3];
};

// 인덱스별 색 지정 함수
float4 DebugCSMColor(uint idx)
{
    if (idx == 0)
    {
        return float4(1, 0, 0, 1); // 빨강
    }
    if (idx == 1)
    {
        return float4(0, 1, 0, 1); // 초록
    }
    if (idx == 2)
    {
        return float4(0, 0, 1, 1); // 파랑
    }
    return float4(1, 1, 1, 1); // 나머지 – 흰색
}

StructuredBuffer<FPointLightInfo> gPointLights : register(t10);
StructuredBuffer<FSpotLightInfo> gSpotLights   : register(t11);

StructuredBuffer<uint> PerTilePointLightIndexBuffer : register(t12);
StructuredBuffer<uint> PerTileSpotLightIndexBuffer  : register(t13);

// Begin Shadow
SamplerComparisonState ShadowSamplerCmp : register(s10);
SamplerState ShadowPointSampler : register(s11);

Texture2DArray SpotShadowMapArray : register(t50);
Texture2DArray DirectionShadowMapArray : register(t51);
TextureCubeArray PointShadowMapArray : register(t52);

uint GetCascadeIndex(float ViewDepth)
{
    // viewDepth 는 LightSpace 깊이(z) 또는 NDC 깊이 복원 뷰 깊이

    for (uint i = 0; i < MAX_CASCADE_NUM; ++i)
    {
        // splits 배열에는 [0]=near, [N]=far 까지 로그 스플릿 저장됨 ex)0..2..4..46..1000
        if (ViewDepth <= CascadeSplits[i + 1])
        {
            return i;
        }
    }
    return MAX_CASCADE_NUM - 1;
}

// NdcDepthToViewDepth
float N2V(float ndcDepth, matrix invProj)
{
    float4 pointView = mul(float4(0, 0, ndcDepth, 1), invProj);
    return pointView.z / pointView.w;
}

float PCF_Filter(float2 uv, float zReceiverNdc, float filterRadiusUV, uint csmIndex)
{
    float sum = 0.0f;
    [unroll]
    for (int i = 0; i < 64; ++i)
    {
        // TODO (offset, 0)에 배열(slice) 인덱스 넣기
        float2 offset = diskSamples64[i] * filterRadiusUV;
        sum += DirectionShadowMapArray.SampleCmpLevelZero(ShadowSamplerCmp, float3(uv + offset, csmIndex), zReceiverNdc);
    }
    return sum / 64;
}

void FindBlocker(out float avgBlockerDepthView, out float numBlockers, float2 uv,
                 float zReceiverView, Texture2DArray DirectionShadowMapArray, matrix InvProj, float LightRadiusWorld, uint csmIndex, FDirectionalLightInfo LightInfo)
{
    float LightRadiusUV = LightRadiusWorld / LightInfo.OrthoWidth; // TO FIX!
    float searchRadius = LightRadiusUV * (zReceiverView - NEAR_PLANE) / zReceiverView; // TO FIX! NearPlane
    float blockerSum = 0;
    numBlockers = 0;

    for (int i = 0; i < 64; ++i)
    {
        // TODO : slice index 받아야 함 (searchRadius , 배열 인덱스)
        float ShadowMapDepth = DirectionShadowMapArray.SampleLevel(ShadowPointSampler, float3(uv + diskSamples64[i] * searchRadius, csmIndex), 0).r;
        ShadowMapDepth = N2V(ShadowMapDepth, InvProj);
        if (ShadowMapDepth < zReceiverView)
        {
            blockerSum += ShadowMapDepth;
            numBlockers += 1;
        }
    }
    avgBlockerDepthView = (numBlockers > 0) ? (blockerSum / numBlockers) : 0.0f;
}

// TODO: 사용하려면 계산식을 수정할 필요가 있음.
float PCSS(float2 UV, float ZReceiverNDC, Texture2DArray DirectionShadowMapArray, matrix ShadowInvProj, float LightRadiusWorld, uint CsmIndex, FDirectionalLightInfo LightInfo)
{
    float LightRadiusUV = LightRadiusWorld / 2.0; // TO FIX!
    float ZReceiverView = N2V(ZReceiverNDC, CascadedInvProj[CsmIndex]);


    // 1. Blocker Search
    float AvgBlockerDepthView = 0;
    float NumBlockers = 0;

    FindBlocker(AvgBlockerDepthView, NumBlockers, UV, ZReceiverView, DirectionShadowMapArray, CascadedInvProj[CsmIndex], LightRadiusWorld, CsmIndex, LightInfo);


    if (NumBlockers < 1)
    {
        // There are no Occluders so early out(this saves filtering)
        return 1.0f;
    }
    else
    {
        // 2. Penumbra Size
        float PenumbraRatio = (ZReceiverView - AvgBlockerDepthView) / AvgBlockerDepthView;
        float FilterRadiusUV = PenumbraRatio * LightRadiusUV * NEAR_PLANE / ZReceiverView; // TO FIX!!!!

        // 3. Filtering
        return PCF_Filter(UV, ZReceiverNDC, FilterRadiusUV, CsmIndex);
    }
}

float CalculateDirectionalShadowFactor(float3 WorldPosition, float3 WorldNormal, FDirectionalLightInfo LightInfo, // 라이트 정보 전체 전달
                                Texture2DArray DirectionShadowMapArray,
                                SamplerComparisonState ShadowSampler)
{
    if (LightInfo.CastShadows == 0)
    {
        return 1.0f;
    }
    
    float ShadowFactor = 1.0;
    float NdotL = dot(normalize(WorldNormal), LightInfo.Direction);
    float bias = 0.01f;
    
    // 1. Project World Position to Light Screen Space
    float4 LightScreen = mul(float4(WorldPosition, 1.0f), LightInfo.LightViewProj);
    LightScreen.xyz /= LightScreen.w; // Perspective Divide -> [-1, 1] 범위로 변환

    // 2. 광원 입장의 Texture 좌표계로 변환
    float2 ShadowMapTexCoord = { LightScreen.x, -LightScreen.y }; // NDC 좌표계와 UV 좌표계는 Y축 방향이 반대
    ShadowMapTexCoord += 1.0;
    ShadowMapTexCoord *= 0.5;

    float LightDistance = LightScreen.z;
    LightDistance -= bias;


    float4 posCam = mul(float4(WorldPosition, 1), ViewMatrix);
    float depthCam = posCam.z; 
    uint CsmIndex = GetCascadeIndex(depthCam);
    //CsmIndex = 1;
    float4 posLS = mul(float4(WorldPosition, 1), CascadedViewProj[CsmIndex]);
    float2 uv = posLS.xy * 0.5f + 0.5f;
    uv.y = 1 - uv.y;
    float zReceiverNdc = posLS.z -= bias;
    ShadowFactor = DirectionShadowMapArray.SampleCmpLevelZero(ShadowSamplerCmp, float3(uv, CsmIndex), zReceiverNdc);

    //ShadowFactor = PCSS(uv, zReceiverNdc, DirectionShadowMapArray, CascadedInvViewProj[CsmIndex], LIGHT_RADIUS_WORLD, CsmIndex, LightInfo);
    return ShadowFactor;
}

int GetMajorFaceIndex(float3 Dir)
{
    float3 absDir = abs(Dir);
    if (absDir.x > absDir.y && absDir.x > absDir.z)
    {
        return Dir.x > 0.0f ? 0 : 1;
    }
    if (absDir.y > absDir.z)
    {
        return Dir.y > 0.0f ? 2 : 3;
    }
    return Dir.z > 0.0f ? 4 : 5;
}

float CalculatePointShadowFactor(float3 WorldPosition, FPointLightInfo LightInfo, // 라이트 정보 전체 전달
                                TextureCubeArray ShadowMapArray,
                                SamplerComparisonState ShadowSampler)
{
    // 1) 광원→조각 방향 (큐브맵 샘플링 좌표)
    float3 Dir = normalize(WorldPosition - LightInfo.Position);
    // 2) 해당 face의 뷰·프로젝션 적용
    int face = GetMajorFaceIndex(Dir);
    float4 posCS = mul(float4(WorldPosition, 1.0f), LightInfo.LightViewProj[face]);
    // 3) 클립스페이스 깊이
    float refDepth = posCS.z / posCS.w;
    // 5) 하드웨어 비교 샘플
    float3 SampleDir = normalize(WorldPosition - LightInfo.Position);
    float shadow = ShadowMapArray.SampleCmpLevelZero(ShadowSampler, float4(SampleDir, (float)LightInfo.ShadowMapArrayIndex), refDepth - LightInfo.ShadowBias).r;
    return shadow;
}

// 기본적인 그림자 계산 함수 (Directional/Spot 용)
// 하드웨어 PCF (SamplerComparisonState 사용) 예시
float CalculateSpotShadowFactor(float3 WorldPosition, FSpotLightInfo LightInfo, // 라이트 정보 전체 전달
                                Texture2DArray ShadowMapArray,
                                SamplerComparisonState ShadowSampler)
{
    // if (!LightInfo.CastShadows)
    // {
    //     return 1.0f; // 그림자 안 드리움
    // }

    // 1 & 2. 라이트 클립 공간 좌표 계산
    float4 PixelPosLightClip = mul(float4(WorldPosition, 1.0f), LightInfo.LightViewProj);

    // 3. 섀도우 맵 UV 계산 [0, 1]
    float2 ShadowMapUV = PixelPosLightClip.xy / PixelPosLightClip.w;
    ShadowMapUV = ShadowMapUV * float2(0.5, -0.5) + 0.5; // Y 반전 필요시

    // 4. 현재 깊이 계산
    float CurrentDepth = PixelPosLightClip.z / PixelPosLightClip.w;

    // UV 범위 체크 (라이트 범위 밖)
    if (any(ShadowMapUV < 0.0f) || any(ShadowMapUV > 1.0f))
    {
        return 1.0f;
    }

    // 5 & 6. 배열의 특정 슬라이스 샘플링 및 비교
    float ShadowFactor = ShadowMapArray.SampleCmpLevelZero(
        ShadowSampler,
        float3(ShadowMapUV, (float)LightInfo.ShadowMapArrayIndex), // UV와 배열 인덱스 사용
        CurrentDepth - LightInfo.ShadowBias  // 바이어스 적용
    );

    return ShadowFactor;
}
// End Shadow

float GetDistanceAttenuation(float Distance, float Radius)
{
    float  InvRadius = 1.0 / Radius;
    float  DistSqr = Distance * Distance;
    float  RadiusMask = saturate(1.0 - DistSqr * InvRadius * InvRadius);
    RadiusMask *= RadiusMask;
    
    return RadiusMask / (DistSqr + 1.0);
}

float GetSpotLightAttenuation(float Distance, float Radius, float3 LightDir, float3 SpotDir, float InnerRadius, float OuterRadius)
{
    float DistAtten = GetDistanceAttenuation(Distance, Radius);
    
    float  CosTheta = dot(SpotDir, -LightDir);
    float  SpotMask = saturate((CosTheta - cos(OuterRadius)) / (cos(InnerRadius) - cos(OuterRadius)));
    SpotMask *= SpotMask;
    
    return DistAtten * SpotMask;
}

////////
/// Diffuse
////////
#define PI 3.14159265359

#ifdef LIGHTING_MODEL_PBR
inline float SchlickWeight(float u) // (1‑u)^5
{
    float m = saturate(1.0 - u);
    return m * m * m * m * m;
}

float DisneyDiffuse(float3 N, float3 L, float3 V, float Roughness)
{
    float3 H = normalize(L + V);
    float  NdotL = saturate(dot(N, L));
    float  NdotV = saturate(dot(N, V));
    float  LdotH2 = saturate(dot(L, H)) * saturate(dot(L, H));

    float  Fd90 = 0.5 + 2.0 * Roughness * LdotH2; // grazing boost
    float  Fd = (1.0 + (Fd90 - 1.0) * SchlickWeight(NdotL)) * (1.0 + (Fd90 - 1.0) * SchlickWeight(NdotV));

    return (Fd * NdotL / PI);
}
#endif

float LambertDiffuse(float3 N, float3 L)
{
    return saturate(dot(N, L)) / PI;
}

////////
/// Specular
////////
float3 F_Schlick(float3 F0, float LdotH)
{
    return F0 + (1.0 - F0) * pow(1.0 - LdotH, 5.0);
}

#ifdef LIGHTING_MODEL_PBR
float  D_GGX(float NdotH, float alpha)
{
    float a2 = alpha * alpha;
    float d = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (PI * d * d);                 // Trowbridge‑Reitz
}

float  G_Smith(float NdotV, float NdotL, float alpha)
{
    float k = alpha * 0.5 + 0.0001;           // Schlick‑GGX (≈α/2)
    float gV = NdotV / (NdotV * (1.0 - k) + k);
    float gL = NdotL / (NdotL * (1.0 - k) + k);
    return gV * gL;
}

float3 CookTorranceSpecular(
    float3  F0,        // base reflectance (metallic → albedo)
    float   Roughness, // 0 = mirror, 1 = diffuse
    float3  N, float3  V, float3  L,
    out float3 OutFresnel
)
{
    float3  H = normalize(V + L);
    float   NdotL = saturate(dot(N, L));
    float   NdotV = saturate(dot(N, V));
    float   NdotH = saturate(dot(N, H));
    float   LdotH = saturate(dot(L, H));

    float   alpha = max(0.001, Roughness * Roughness);

    float   D = D_GGX(NdotH, alpha);
    float   G = G_Smith(NdotV, NdotL, alpha);
    float3  F = F_Schlick(F0, LdotH);
    OutFresnel = F;

    return (D * G * F) * (NdotL / (4.0 * NdotV * NdotL + 1e-5));
}
#else
float CalculateSpecular(float3 N, float3 L, float3 V, float Shininess, float SpecularStrength = 1.0)
{
#ifdef LIGHTING_MODEL_GOURAUD
    float3 R = reflect(-L, N);
    float Spec = pow(max(dot(V, R), 0.0), Shininess);
#else
    float3 H = normalize(L + V); // Blinn-Phong
    float NdotH = saturate(dot(N, H));
    float Spec = pow(NdotH, Shininess);
#endif
    return Spec * SpecularStrength;
}
#endif

////////
/// BRDF
////////
struct FBRDFResult
{
    float3 Color; // (Diffuse + Specular)
    float3 SpecularContribution; // 알파 계산 시 하이라이트 강도 측정 용도
    float FresnelFactor; // [0, 1]
};

FBRDFResult CalculateBRDF(float3 L, float3 V, float3 N,
#ifdef LIGHTING_MODEL_PBR
    float3 BaseColor, float Metallic, float Roughness
#else
    float3 DiffuseColor, float3 SpecularColor, float Shininess
#endif
)
{
    FBRDFResult Result = (FBRDFResult)0;
    
    float3 Diffuse = float3(0.0, 0.0, 0.0);
    float3 Specular = float3(0.0, 0.0, 0.0);

#ifdef LIGHTING_MODEL_PBR
    float3 F0 = lerp(0.04, BaseColor, Metallic);

    float KdScale = 1.0 - Metallic;
    float3 Kd = BaseColor * KdScale;

    Diffuse = DisneyDiffuse(N, L, V, Roughness) * Kd;

    float3 SpecularFresnel;
    Specular = CookTorranceSpecular(F0, Roughness, N, V, L, SpecularFresnel);

    Result.SpecularContribution = Specular / max(saturate(dot(N, L)), 1e-5); // 에너지 보존
    Result.FresnelFactor = SpecularFresnel.r;
#else
    Diffuse = LambertDiffuse(N, L) * DiffuseColor;
#ifdef LIGHTING_MODEL_BLINN_PHONG
    Specular = CalculateSpecular(N, L, V, Shininess) * SpecularColor;
    Result.SpecularContribution = Specular;
    /**
     * TODO: Blinn-Phong을 위해 Fresnel의 근사치를 계산해 줄 수 있음.
     * e.g.
     *   float3 F0 = SpecularColor; // 또는 float3(0.04, 0.04, 0.04);
     *   Result.FresnelFactor = F_Schlick(F0, saturate(dot(N, V))).r; // NdotV는 시야각 기반, NdotL은 조명 기반. Phong에서는 시야각 기반으로.
     */
#endif
#endif
    Result.Color = Diffuse + Specular;
    
    return Result;
}

////////
/// Calculate Light
////////
struct FLightOutput
{
    float3 Color; // 최종 픽셀에 더해질 색상 (Diffuse + Specular)
    float3 SpecularIntensity; // 광원에 의한 스페큘러 에너지 (조명의 어테뉴에이션 및 그림자가 계산된 결과)
    float LightFresnelFactor;
};

FLightOutput PointLight(int Index, float3 WorldPosition, float3 WorldNormal, float3 WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
    float3 BaseColor, float Metallic, float Roughness
#else
    float3 DiffuseColor, float3 SpecularColor, float Shininess
#endif
)
{
    FLightOutput Output = (FLightOutput)0;
    
    FPointLightInfo LightInfo = gPointLights[Index];
    
    float3 ToLight = LightInfo.Position - WorldPosition;
    float Distance = length(ToLight);
    
    float Attenuation = GetDistanceAttenuation(Distance, LightInfo.Radius);
    if (Attenuation <= 0.0)
    {
        return Output;
    }
    
    // --- 그림자 계산
    float Shadow = 1.0;
    if (LightInfo.CastShadows && IsShadow)
    {
        // 그림자 계산
        Shadow = CalculatePointShadowFactor(WorldPosition, LightInfo, PointShadowMapArray, ShadowSamplerCmp);
        // 그림자 계수가 0 이하면 더 이상 계산 불필요
        if (Shadow <= 0.0)
        {
            return Output;
        }
    }
    
    float3 L = normalize(ToLight);
    float3 V = normalize(WorldViewPosition - WorldPosition);

    FBRDFResult BRDF = CalculateBRDF(L, V, WorldNormal,
#ifdef LIGHTING_MODEL_PBR
        BaseColor, Metallic, Roughness
#else
        DiffuseColor, SpecularColor, Shininess
#endif
    );

    float3 LightEnergy = LightInfo.LightColor.rgb * LightInfo.Intensity;

    Output.Color = BRDF.Color * LightEnergy * Attenuation * Shadow;
    Output.SpecularIntensity = BRDF.SpecularContribution * LightEnergy * Attenuation * Shadow;
    Output.LightFresnelFactor = BRDF.FresnelFactor;

    return Output;
}

FLightOutput SpotLight(int Index, float3 WorldPosition, float3 WorldNormal, float3 WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
    float3 BaseColor, float Metallic, float Roughness
#else
    float3 DiffuseColor, float3 SpecularColor, float Shininess
#endif
)
{
    FLightOutput Output = (FLightOutput)0;
    
    FSpotLightInfo LightInfo = gSpotLights[Index];
    
    float3 ToLight = LightInfo.Position - WorldPosition;
    float Distance = length(ToLight);
    float3 LightDir = normalize(ToLight);
    
    float SpotlightFactor = GetSpotLightAttenuation(Distance, LightInfo.Radius, LightDir, normalize(LightInfo.Direction), LightInfo.InnerRad, LightInfo.OuterRad);
    if (SpotlightFactor <= 0.0)
    {
        return Output;
    }

    // --- 그림자 계산
    float Shadow = 1.0;
    if (LightInfo.CastShadows && IsShadow)
    {
        // 그림자 계산
        Shadow  = CalculateSpotShadowFactor(WorldPosition, LightInfo, SpotShadowMapArray, ShadowSamplerCmp);
        // 그림자 계수가 0 이하면 더 이상 계산 불필요
        if (Shadow <= 0.0)
        {
            return Output;
        }
    }

    float3 L = normalize(ToLight);
    float3 V = normalize(WorldViewPosition - WorldPosition);
    
    FBRDFResult BRDF = CalculateBRDF(L, V, WorldNormal,
#ifdef LIGHTING_MODEL_PBR
        BaseColor, Metallic, Roughness
#else
        DiffuseColor, SpecularColor, Shininess
#endif
    );
    
    float3 LightEnergy = LightInfo.LightColor.rgb * LightInfo.Intensity;

    Output.Color = BRDF.Color * LightEnergy * SpotlightFactor * Shadow;
    Output.SpecularIntensity = BRDF.SpecularContribution * LightEnergy * SpotlightFactor * Shadow;
    Output.LightFresnelFactor = BRDF.FresnelFactor;

    return Output;
}

FLightOutput DirectionalLight(int Index, float3 WorldPosition, float3 WorldNormal, float3 WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
    float3 BaseColor, float Metallic, float Roughness
#else
    float3 DiffuseColor, float3 SpecularColor, float Shininess
#endif
)
{
    FLightOutput Output = (FLightOutput)0;
    
    FDirectionalLightInfo LightInfo = Directional[Index];
    
    float4 posCam = mul(float4(WorldPosition, 1), ViewMatrix);
    float depthCam = posCam.z / posCam.w;
    uint csmIndex = GetCascadeIndex(depthCam); // 시각적 디버깅용
    
    // --- 그림자 계산
    float Shadow = 1.0;
    if (IsShadow)
    {
        Shadow = CalculateDirectionalShadowFactor(WorldPosition, WorldNormal, LightInfo, DirectionShadowMapArray, ShadowSamplerCmp);
        // 그림자 계수가 0 이하면 더 이상 계산 불필요
        if (Shadow <= 0.0)
        {
            return Output;
        }
    }

    float3 L = normalize(-LightInfo.Direction);
    float3 V = normalize(WorldViewPosition - WorldPosition);
    
    FBRDFResult BRDF = CalculateBRDF(L, V, WorldNormal,
#ifdef LIGHTING_MODEL_PBR
        BaseColor, Metallic, Roughness
#else
        DiffuseColor, SpecularColor, Shininess
#endif
    );

    float3 LightEnergy = LightInfo.LightColor.rgb * LightInfo.Intensity;

    Output.Color = BRDF.Color * LightEnergy * Shadow /* DebugCSMColor(csmIndex) */ ;
    Output.SpecularIntensity = BRDF.SpecularContribution * LightEnergy * Shadow;
    Output.LightFresnelFactor = BRDF.FresnelFactor;

    return Output;
}


/**
 * RGB 색상 값을 인간이 인지하는 밝기로 변환하기 위한 가중치.
 * Rec.709(ITU-R BT.709) 표준.
 */
float3 LUMINANCE = float3(0.299, 0.587, 0.114);


float4 Lighting(float3 WorldPosition, float3 WorldNormal, float3 WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
    float3 BaseColor, float Metallic, float Roughness,
#else
    float3 DiffuseColor, float3 SpecularColor, float Shininess,
#endif
    float BaseAlpha,
    uint TileIndex
)
{
    float3 FinalColorAccumulator = float3(0.0, 0.0, 0.0);
    float MaxObservedSpecularLuminance = 0.0f; // 보이는 스페큘러 중 최대 밝기
    float ViewBasedFresnel = 0.0f;             // 시야각 기반 Fresnel (NdotV)

    float3 V = normalize(WorldViewPosition - WorldPosition);

    // 시야각 기반 Fresnel 계산 (모든 셰이딩 모델 공통으로 사용 가능)
#ifdef LIGHTING_MODEL_PBR
    float3 F0_view = lerp(0.04, BaseColor, Metallic);
#else
    float3 F0_view = SpecularColor; // Phong에서는 Specular 색상을 F0로 사용하거나 고정값 사용. 또는 float3(0.04, 0.04, 0.04);
#endif
    ViewBasedFresnel = F_Schlick(F0_view, saturate(dot(WorldNormal, V))).r; // 대표값 (.r 또는 평균)

    
    // 조명 계산
    int BucketsPerTile = MAX_LIGHT_PER_TILE / 32;
    int StartIndex = TileIndex * BucketsPerTile;
    for (int Bucket = 0; Bucket < BucketsPerTile; ++Bucket)
    {
        int PointMask = PerTilePointLightIndexBuffer[StartIndex + Bucket];
        int SpotMask = PerTileSpotLightIndexBuffer[StartIndex + Bucket];
        for (int bit = 0; bit < 32; ++bit)
        {
            if (PointMask & (1u << bit))
            {
                // 전역 조명 인덱스는 bucket * 32 + bit 로 계산됨.
                // 전역 조명 인덱스가 총 조명 수보다 작은 경우에만 추가
                int GlobalPointLightIndex = Bucket * 32 + bit;
                if (GlobalPointLightIndex < MAX_LIGHT_PER_TILE) // TODO: MAX_LIGHT_PER_TILE 대신 실제 포인트 라이트 개수와 비교해야 함. (중요)
                {
                    FLightOutput Result = PointLight(
                        GlobalPointLightIndex, WorldPosition, WorldNormal, WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
                        BaseColor, Metallic, Roughness
#else
                        DiffuseColor, SpecularColor, Shininess
#endif
                    );
                    FinalColorAccumulator += Result.Color;
                    MaxObservedSpecularLuminance = max(MaxObservedSpecularLuminance, dot(Result.SpecularIntensity, LUMINANCE));
                }
            }
            if (SpotMask & (1u << bit))
            {
                int GlobalSpotLightIndex = Bucket * 32 + bit;
                if (GlobalSpotLightIndex < MAX_LIGHT_PER_TILE) // TODO: MAX_LIGHT_PER_TILE 대신 실제 스팟 라이트 개수와 비교해야 함. (중요)
                {
                    FLightOutput Result = SpotLight(
                        GlobalSpotLightIndex, WorldPosition, WorldNormal, WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
                        BaseColor, Metallic, Roughness
#else
                        DiffuseColor, SpecularColor, Shininess
#endif
                    );
                    FinalColorAccumulator += Result.Color;
                    MaxObservedSpecularLuminance = max(MaxObservedSpecularLuminance, dot(Result.SpecularIntensity, LUMINANCE));
                }
            }
        }
    }
    
    [unroll(MAX_DIRECTIONAL_LIGHT)]
    for (int k = 0; k < 1; k++) // TODO: 그림자는 0번 인덱스만 사용하더라도 실제 디렉셔널 라이트 개수만큼 루프해야함. (중요)
    {
         FLightOutput Result = DirectionalLight(
            k, WorldPosition, WorldNormal, WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
            BaseColor, Metallic, Roughness
#else
            DiffuseColor, SpecularColor, Shininess
#endif
        );
        FinalColorAccumulator += Result.Color;
        MaxObservedSpecularLuminance = max(MaxObservedSpecularLuminance, dot(Result.SpecularIntensity, LUMINANCE));
    }

    /*
    // 최종 알파 계산
    // ViewBasedFresnel (시야각)과 MaxObservedSpecularLuminance (실제 하이라이트 강도)를 결합
    // MaxObservedSpecularLuminance의 스케일 조절이 필요할 수 있음 (예: * 0.2f ~ 1.0f)
    // 이 값은 [0, 거의 무한대] 범위일 수 있으므로 saturate(MaxObservedSpecularLuminance * factor) 등으로 [0,1] 매핑
    float HighlightFactor = saturate(MaxObservedSpecularLuminance * 1.0); // 0.5f는 실험적 가중치
    
    float FinalFresnelInfluence = max(ViewBasedFresnel, HighlightFactor); // 둘 중 더 강한 반사 요인을 선택
    // 또는 ViewBasedFresnel + highlightFactor * (1.0 - ViewBasedFresnel) 같은 블렌딩
    
    float EffectiveOpacity = FinalFresnelInfluence + (1.0 - FinalFresnelInfluence) * BaseAlpha;
    float FinalAlpha = saturate(EffectiveOpacity);
    */

    /*
    float highlightThresholdLow = 0.2f;
    float highlightThresholdHigh = 1.0f; // 이 값을 넘어서는 밝기는 모두 최대 영향
    float normalizedHighlightStrength = saturate( (MaxObservedSpecularLuminance - highlightThresholdLow) / (highlightThresholdHigh - highlightThresholdLow) );

    float opacityFromHighlight = pow(normalizedHighlightStrength, 2.0f); // [0, 1]

    float fresnelContribution = ViewBasedFresnel * normalizedHighlightStrength * 0.5f; // 0.5는 가중치

    float combinedOpacity = BaseAlpha + (1.0 - BaseAlpha) * opacityFromHighlight; // 하이라이트로 BaseAlpha에서 1까지 보간
    combinedOpacity = saturate(combinedOpacity + fresnelContribution); // Fresnel 기여분 추가 (1을 넘지 않도록)

    */
    float FinalAlpha = BaseAlpha;
    
    // 앰비언트
#ifdef LIGHTING_MODEL_PBR
    float3 IBL_DiffuseColor = float3(0.03, 0.03, 0.03); // TODO: 임시 값으로, 추후 IBL 적용

    if (AmbientLightsCount > 0)
    {
        IBL_DiffuseColor = Ambient[0].AmbientColor.rgb;
    }
    FinalColorAccumulator += BaseColor * (1.0 - Metallic) * IBL_DiffuseColor;
#else
    float3 AmbientLightColor = float3(0.025, 0.025, 0.025);
    
    if (AmbientLightsCount > 0)
    {
        AmbientLightColor = Ambient[0].AmbientColor.rgb;
    }
    FinalColorAccumulator += DiffuseColor * AmbientLightColor;
#endif
    
    return float4(FinalColorAccumulator * FinalAlpha, FinalAlpha);
    //return float4(FinalColorAccumulator, FinalAlpha);
}


float4 Lighting(float3 WorldPosition, float3 WorldNormal, float3 WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
    float3 BaseColor, float Metallic, float Roughness,
#else
    float3 DiffuseColor, float3 SpecularColor, float Shininess,
#endif
    float BaseAlpha
)
{
    float3 FinalColorAccumulator = float3(0.0, 0.0, 0.0);
    float MaxObservedSpecularLuminance = 0.0f; // 보이는 스페큘러 중 최대 밝기
    float ViewBasedFresnel = 0.0f;             // 시야각 기반 Fresnel (NdotV)

    float3 V = normalize(WorldViewPosition - WorldPosition);

    // 시야각 기반 Fresnel 계산 (모든 셰이딩 모델 공통으로 사용 가능)
#ifdef LIGHTING_MODEL_PBR
    float3 F0_view = lerp(0.04, BaseColor, Metallic);
#else
    float3 F0_view = SpecularColor; // Phong에서는 Specular 색상을 F0로 사용하거나 고정값 사용. 또는 float3(0.04, 0.04, 0.04);
#endif
    ViewBasedFresnel = F_Schlick(F0_view, saturate(dot(WorldNormal, V))).r; // 대표값 (.r 또는 평균)

    // 조명 계산
    // 다소 비효율적일 수도 있음.
    [unroll(MAX_POINT_LIGHT)]
    for (int i = 0; i < PointLightsCount; i++)
    {
        FLightOutput Result = PointLight(
            i, WorldPosition, WorldNormal, WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
            BaseColor, Metallic, Roughness
#else
            DiffuseColor, SpecularColor, Shininess
#endif
        );
        FinalColorAccumulator += Result.Color;
        MaxObservedSpecularLuminance = max(MaxObservedSpecularLuminance, dot(Result.SpecularIntensity, LUMINANCE));
    }

    [unroll(MAX_SPOT_LIGHT)]
    for (int j = 0; j < SpotLightsCount; j++)
    {
        FLightOutput Result = SpotLight(
            j, WorldPosition, WorldNormal, WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
            BaseColor, Metallic, Roughness
#else
            DiffuseColor, SpecularColor, Shininess
#endif
        );
        FinalColorAccumulator += Result.Color;
        MaxObservedSpecularLuminance = max(MaxObservedSpecularLuminance, dot(Result.SpecularIntensity, LUMINANCE));
    }
    
    [unroll(MAX_DIRECTIONAL_LIGHT)]
    for (int k = 0; k < 1; k++) 
    {
        FLightOutput Result = DirectionalLight(
            k, WorldPosition, WorldNormal, WorldViewPosition,
#ifdef LIGHTING_MODEL_PBR
            BaseColor, Metallic, Roughness
#else
            DiffuseColor, SpecularColor, Shininess
#endif
        );
        FinalColorAccumulator += Result.Color;
        MaxObservedSpecularLuminance = max(MaxObservedSpecularLuminance, dot(Result.SpecularIntensity, LUMINANCE));
    }

    
    // 최종 알파 계산
    // ViewBasedFresnel (시야각)과 MaxObservedSpecularLuminance (실제 하이라이트 강도)를 결합
    // MaxObservedSpecularLuminance의 스케일 조절이 필요할 수 있음 (예: * 0.2f ~ 1.0f)
    // 이 값은 [0, 거의 무한대] 범위일 수 있으므로 saturate(MaxObservedSpecularLuminance * factor) 등으로 [0,1] 매핑
    float HighlightFactor = saturate(MaxObservedSpecularLuminance * 0.5f); // 0.5f는 실험적 가중치
    
    float FinalFresnelInfluence = max(ViewBasedFresnel, HighlightFactor); // 둘 중 더 강한 반사 요인을 선택
    // 또는 ViewBasedFresnel + highlightFactor * (1.0 - ViewBasedFresnel) 같은 블렌딩
    
    float EffectiveOpacity = FinalFresnelInfluence + (1.0 - FinalFresnelInfluence) * BaseAlpha;
    float FinalAlpha = saturate(EffectiveOpacity);


    // 앰비언트
#ifdef LIGHTING_MODEL_PBR
    float3 IBL_DiffuseColor = float3(0.03, 0.03, 0.03); // TODO: 임시 값으로, 추후 IBL 적용

    if (AmbientLightsCount > 0)
    {
        IBL_DiffuseColor = Ambient[0].AmbientColor.rgb;
    }
    FinalColorAccumulator += BaseColor * (1.0 - Metallic) * IBL_DiffuseColor;
#else
    float3 AmbientLightColor = float3(0.01, 0.01, 0.01);
    
    if (AmbientLightsCount > 0)
    {
        AmbientLightColor = Ambient[0].AmbientColor.rgb;
    }
    FinalColorAccumulator += DiffuseColor * AmbientLightColor;
#endif
    
    return float4(FinalColorAccumulator * FinalAlpha, FinalAlpha);
}
