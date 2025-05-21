#pragma once
#include "ParticleModuleSizeBase.h"
#include "Distribution/DistributionVector.h"

class UParticleModuleSize : public UParticleModuleSizeBase
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModuleSizeBase)
public:
    UParticleModuleSize();

    virtual void DisplayProperty() override;

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, StartSize)
};
