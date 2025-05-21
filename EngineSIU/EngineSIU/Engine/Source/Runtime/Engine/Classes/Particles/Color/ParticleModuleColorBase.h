#pragma once
#include "Distribution/DistributionVector.h"
#include "Distribution/DistributionFloat.h"
#include "Particles/ParticleModule.h"

class UParticleModuleColorBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleColorBase, UParticleModule)
public:
    UParticleModuleColorBase();

    // Initial Color : When particle is spawn
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, StartColor)

    // Initial Alpha : When particle is spawn
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionFloat, StartAlpha)

public:
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;

    virtual void DisplayProperty() override;
};
