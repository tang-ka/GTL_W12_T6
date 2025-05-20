#pragma once
#include "ParticleModuleSizeBase.h"
#include "Distribution/DistributionVector.h"

class UParticleModuleSize : public UParticleModuleSizeBase
{
    DECLARE_CLASS(UParticleModuleSize, UParticleModuleSizeBase)
public:
    UParticleModuleSize() = default;

    virtual void DisplayProperty() override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, StartSize)
};
