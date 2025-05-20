#pragma once
#include "ParticleModuleLocationBase.h"
#include "Classes/Distribution/DistributionVector.h"

class UParticleModuleLocation : public UParticleModuleLocationBase
{
    DECLARE_CLASS(UParticleModuleLocation, UParticleModuleLocationBase)
public:
    UParticleModuleLocation();
    virtual ~UParticleModuleLocation() override = default;

    virtual void DisplayProperty() override;

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, StartLocation)
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, LocationOffset)
};

