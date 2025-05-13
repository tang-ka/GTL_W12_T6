#pragma once

#include "Define.h"
#include "Hal/PlatformType.h"
#include "Container/Array.h"

struct FStaticMeshVertex
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
    float NormalX, NormalY, NormalZ;
    float TangentX, TangentY, TangentZ, TangentW;
    float U = 0, V = 0;
    uint32 MaterialIndex;

    friend FArchive& operator<<(FArchive& Ar, FStaticMeshVertex& Data)
    {
        return Ar << Data.X << Data.Y << Data.Z
                  << Data.R << Data.G << Data.B << Data.A
                  << Data.NormalX << Data.NormalY << Data.NormalZ
                  << Data.TangentX << Data.TangentY << Data.TangentZ << Data.TangentW
                  << Data.U << Data.V
                  << Data.MaterialIndex;
    }
};

struct FStaticMeshRenderData
{
    FWString ObjectName;
    FString DisplayName;

    TArray<FStaticMeshVertex> Vertices;
    TArray<UINT> Indices;

    TArray<FMaterialInfo> Materials;
    TArray<FMaterialSubset> MaterialSubsets;

    FVector BoundingBoxMin;
    FVector BoundingBoxMax;

    void Serialize(FArchive& Ar)
    {
        FString ObjectNameStr = ObjectName;

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
