#include "DistributionFloat.h"

void FDistributionFloat::GetOutRange(float& MinOut, float& MaxOut)
{
    if (MinValue > MaxValue)
    {
        MaxValue = MinValue;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(MinValue, MaxValue);
    
    float RandValue1 = dis(gen);
    float RandValue2 = dis(gen);
    
    MinOut = std::min(RandValue1, RandValue2);
    MaxOut = std::max(RandValue1, RandValue2);
}

float FDistributionFloat::GetValue()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    float TempMin, TempMax;
    GetOutRange(TempMin, TempMax);
    std::uniform_real_distribution<float> dis(TempMin, TempMax);
    
    return dis(gen);
}
