#pragma once
#include "ParticleModuleAccelerationBase.h"

class UParticleModuleAcceleration :public UParticleModuleAccelerationBase
{
    DECLARE_CLASS(UParticleModuleAcceleration, UParticleModuleAccelerationBase)
public:
    UParticleModuleAcceleration();
    virtual ~UParticleModuleAcceleration() override = default;
    virtual void DisplayProperty() override;

    virtual void FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, Acceleration)
};

