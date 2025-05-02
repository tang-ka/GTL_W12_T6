#pragma once

#include "Hal/PlatformType.h"

struct FSkeletalMeshVertex
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
    float NormalX, NormalY, NormalZ;
    float TangentX, TangentY, TangentZ, TangentW;
    float U = 0, V = 0;
    uint32 BoneIndices[4];
    float BoneWeights[4];
};
