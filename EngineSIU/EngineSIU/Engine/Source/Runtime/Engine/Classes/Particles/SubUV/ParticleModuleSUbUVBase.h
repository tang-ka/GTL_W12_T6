#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleSUbUVBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSUbUVBase, UParticleModule)
public:
    UParticleModuleSUbUVBase() = default;

    virtual void DisplayProperty() override {}

    virtual EModuleType	GetModuleType() const override { return EModuleType::EPMT_SubUV; }
};
