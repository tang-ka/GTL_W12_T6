#include "DistributionVector.h"

void FDistributionVector::GetOutRange(FVector& MinOut, FVector& MaxOut)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // X, Y, Z 각 컴포넌트별로 랜덤 값 생성
    std::uniform_real_distribution<float> disX(MinValue.X, MaxValue.X);
    std::uniform_real_distribution<float> disY(MinValue.Y, MaxValue.Y);
    std::uniform_real_distribution<float> disZ(MinValue.Z, MaxValue.Z);
    
    // 각 컴포넌트별로 두 개의 랜덤 값 생성
    float RandX1 = disX(gen), RandX2 = disX(gen);
    float RandY1 = disY(gen), RandY2 = disY(gen);
    float RandZ1 = disZ(gen), RandZ2 = disZ(gen);
    
    // MinOut과 MaxOut 설정 (각 컴포넌트별로 min/max)
    MinOut.X = std::min(RandX1, RandX2);
    MinOut.Y = std::min(RandY1, RandY2);
    MinOut.Z = std::min(RandZ1, RandZ2);
    
    MaxOut.X = std::max(RandX1, RandX2);
    MaxOut.Y = std::max(RandY1, RandY2);
    MaxOut.Z = std::max(RandZ1, RandZ2);
}
