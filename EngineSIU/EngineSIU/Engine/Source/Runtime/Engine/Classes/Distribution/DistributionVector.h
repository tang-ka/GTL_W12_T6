#pragma once
#include "Math/Vector.h"

struct FDistributionVector
{
    FDistributionVector() : MinValue(0), MaxValue(0) {}
    FDistributionVector(FVector MinValue, FVector MaxValue) : MinValue(MinValue), MaxValue(MaxValue) {}
    
    FVector MinValue;
    FVector MaxValue;

    void GetOutRange(FVector& MinOut, FVector& MaxOut);
};
