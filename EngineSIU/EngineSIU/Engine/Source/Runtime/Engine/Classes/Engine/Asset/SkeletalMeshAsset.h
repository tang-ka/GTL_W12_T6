#pragma once

#include "Define.h"
#include "Hal/PlatformType.h"
#include "Container/Array.h"

struct FSkeletalMeshVertex
{
    float X = 0.f, Y = 0.f, Z = 0.f;
    float R = 0.5f, G = 0.5f, B = 0.5f, A = 0.5f;
    float NormalX = 0.f, NormalY = 0.f, NormalZ = 0.f;
    float TangentX = 0.f, TangentY = 0.f, TangentZ = 0.f, TangentW = 0.f;
    float U = 0, V = 0;
    uint32 BoneIndices[4] = { 0, 0, 0, 0 };
    float BoneWeights[4] = { 0.f, 0.f, 0.f, 0.f };
};

struct FSkeletalMeshRenderData
{
    FWString ObjectName;
    FString DisplayName;

    TArray<FSkeletalMeshVertex> Vertices;
    TArray<UINT> Indices;

    TArray<FObjMaterialInfo> Materials;
    TArray<FMaterialSubset> MaterialSubsets;

    FVector BoundingBoxMin;
    FVector BoundingBoxMax;
};
