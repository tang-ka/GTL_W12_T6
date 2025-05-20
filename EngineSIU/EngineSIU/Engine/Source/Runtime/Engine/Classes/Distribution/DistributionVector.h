#pragma once
#include "Math/Vector.h"

struct FDistributionVector
{
    FVector MinValue;
    FVector MaxValue;

    std::mt19937 Gen;
    std::uniform_real_distribution<float> DistX;
    std::uniform_real_distribution<float> DistY;
    std::uniform_real_distribution<float> DistZ;

public:
    FDistributionVector()
        : MinValue(FVector::ZeroVector)
        , MaxValue(FVector::ZeroVector)
        , Gen(std::random_device{}())
        , DistX(MinValue.X, MaxValue.X)
        , DistY(MinValue.Y, MaxValue.Y)
        , DistZ(MinValue.Z, MaxValue.Z)
    {}

    // 범위를 지정하는 생성자
    FDistributionVector(const FVector& InMin, const FVector& InMax)
        : MinValue(InMin)
        , MaxValue(InMax)
        , Gen(std::random_device{}())
        , DistX(MinValue.X, MaxValue.X)
        , DistY(MinValue.Y, MaxValue.Y)
        , DistZ(MinValue.Z, MaxValue.Z)
    {   }

    void GetOutRange(FVector& MinOut, FVector& MaxOut);

    void UpdateDistributionParam()
    {
        DistX = std::uniform_real_distribution<float>(MinValue.X, MaxValue.X);
        DistY = std::uniform_real_distribution<float>(MinValue.Y, MaxValue.Y);
        DistZ = std::uniform_real_distribution<float>(MinValue.Z, MaxValue.Z);
    }

    void UpdateRange(const FVector& InMin, const FVector& InMax)
    {
        MinValue = InMin;
        MaxValue = InMax;
        
        UpdateDistributionParam();
    }

    FVector GetValue()
    {
        UpdateDistributionParam();

        return FVector(
            DistX(Gen),
            DistY(Gen),
            DistZ(Gen)
        );
    }
};
