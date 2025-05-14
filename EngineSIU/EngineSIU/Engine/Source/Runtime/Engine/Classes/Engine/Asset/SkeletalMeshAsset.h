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

    friend FArchive& operator<<(FArchive& Ar, FSkeletalMeshVertex& Data)
    {
        return Ar << Data.X << Data.Y << Data.Z
                  << Data.R << Data.G << Data.B << Data.A
                  << Data.NormalX << Data.NormalY << Data.NormalZ
                  << Data.TangentX << Data.TangentY << Data.TangentZ << Data.TangentW
                  << Data.U << Data.V
                  << Data.BoneIndices[0] << Data.BoneIndices[1] << Data.BoneIndices[2] << Data.BoneIndices[3]
                  << Data.BoneWeights[0] << Data.BoneWeights[1] << Data.BoneWeights[2] << Data.BoneWeights[3];
    }
};

struct FSkeletalMeshRenderData
{
    FWString ObjectName;
    FString DisplayName;

    TArray<FSkeletalMeshVertex> Vertices;
    TArray<UINT> Indices;

    TArray<FMaterialInfo> Materials;
    TArray<FMaterialSubset> MaterialSubsets;

    FVector BoundingBoxMin;
    FVector BoundingBoxMax;

    void Serialize(FArchive& Ar)
    {
        FString ObjectNameStr;
        if (!ObjectName.empty())
        {
            ObjectNameStr = ObjectName;
        }

        Ar << ObjectNameStr
           << DisplayName
           << Vertices
           << Indices
           << Materials
           << MaterialSubsets
           << BoundingBoxMin
           << BoundingBoxMax;

        ObjectName = ObjectNameStr.ToWideString();
    }
};
