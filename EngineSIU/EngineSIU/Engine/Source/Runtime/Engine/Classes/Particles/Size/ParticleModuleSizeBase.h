#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleSizeBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSizeBase, UParticleModule)
public:
    UParticleModuleSizeBase() = default;

    virtual void DisplayProperty() override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseUniformSize)
};
