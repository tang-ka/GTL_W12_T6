#pragma once
#include "Distribution/DistributionVector.h"
#include "Particles/ParticleModule.h"

class UParticleModuleSize : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModule)
public:
    UParticleModuleSize() = default;

    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, StartSize)
};
