#pragma once
#include "Distribution/DistributionFloat.h"
#include "Particles/ParticleModule.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"


class UParticleModuleLifeTime : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleLifeTime, UParticleModule)

public:
    UParticleModuleLifeTime();
    virtual ~UParticleModuleLifeTime() override = default;

    // LifeTime
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionFloat, LifeSpan)

public:
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;

    virtual void DisplayProperty() override;
};
