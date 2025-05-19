#pragma once
#include "ParticleModuleSizeBase.h"
#include "Distribution/DistributionVector.h"

class UParticleModuleSize : public UParticleModuleSizeBase
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModuleSizeBase)
public:
    UParticleModuleSize() = default;

    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, StartSize)
};
