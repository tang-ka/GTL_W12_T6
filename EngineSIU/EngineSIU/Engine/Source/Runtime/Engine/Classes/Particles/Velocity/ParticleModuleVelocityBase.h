#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleVelocityBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleVelocityBase, UParticleModule)
public:
    UParticleModuleVelocityBase() = default;

    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bInWorldSpace)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bApplyOwnerScale)
};
