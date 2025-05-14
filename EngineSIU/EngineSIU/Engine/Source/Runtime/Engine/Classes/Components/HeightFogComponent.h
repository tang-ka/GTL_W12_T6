#pragma once
#include "PrimitiveComponent.h"
#include <Math/Color.h>

class UHeightFogComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UHeightFogComponent, UPrimitiveComponent)
private:
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogDistanceWeight;
    float EndDistance;
    FLinearColor FogInscatteringColor;

public:
    UHeightFogComponent(float Density = 0.5f, float HeightFalloff = 0.05f, float StartDist = 0.f, float EndDist = 0.1f, float DistanceWeight = 0.75f);

    float GetFogDensity() const { return FogDensity; }
    float GetFogHeightFalloff() const { return FogHeightFalloff; }
    float GetStartDistance() const { return StartDistance; }
    float GetFogDistanceWeight() const { return FogDistanceWeight; }
    float GetEndDistance() const { return EndDistance; }
    FLinearColor GetFogColor() const { return FogInscatteringColor; }
    
    void SetFogDensity(float Value);
    void SetFogHeightFalloff(float Value);
    void SetStartDistance(float Value);
    void SetFogDistanceWeight(float Value);
    void SetEndDistance(float Value);
    void SetFogColor(FLinearColor Color);

    virtual UObject* Duplicate(UObject* InOuter) override;
    virtual void GetProperties(TMap<FString, FString>& OutProperties) const override;
    virtual void SetProperties(const TMap<FString, FString>& InProperties) override;
    
};
