#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleVelocityBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleVelocityBase, UParticleModule)
public:
    UParticleModuleVelocityBase() = default;

    virtual void DisplayProperty() override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bInWorldSpace)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bApplyOwnerScale)
};
