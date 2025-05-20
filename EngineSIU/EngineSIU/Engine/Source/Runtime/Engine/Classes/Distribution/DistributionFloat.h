#pragma once
#include "UObject/ObjectMacros.h"

struct FDistributionFloat
{
    FDistributionFloat() : MinValue(1), MaxValue(1) {}
    FDistributionFloat(float InMinValue, float InMaxValue) : MinValue(InMinValue), MaxValue(InMaxValue) {}

    float MinValue;
    float MaxValue;

    void GetOutRange(float& MinOut, float& MaxOut) const;
    float GetValue() const;
};
