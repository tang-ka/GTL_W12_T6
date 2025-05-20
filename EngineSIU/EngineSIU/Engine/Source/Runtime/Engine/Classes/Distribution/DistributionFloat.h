#pragma once
#include "UObject/ObjectMacros.h"

struct FDistributionFloat
{
    FDistributionFloat() : MinValue(0), MaxValue(0) {}
    FDistributionFloat(float InMinValue, float InMaxValue) : MinValue(InMinValue), MaxValue(InMaxValue) {}

    float MinValue;
    float MaxValue;

    void GetOutRange(float& MinOut, float& MaxOut) const;
    float GetValue() const;
};
