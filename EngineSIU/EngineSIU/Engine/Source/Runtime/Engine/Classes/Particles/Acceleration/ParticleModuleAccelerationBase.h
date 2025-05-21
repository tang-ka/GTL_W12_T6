#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleAccelerationBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleAccelerationBase, UParticleModule)
public:
    UParticleModuleAccelerationBase() = default;

    virtual void DisplayProperty() override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bInWorldSpace)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bApplyOwnerScale)
};

