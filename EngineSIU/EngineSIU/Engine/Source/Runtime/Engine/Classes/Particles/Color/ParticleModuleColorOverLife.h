#pragma once
#include "ParticleModuleColorBase.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"


class UParticleModuleColorOverLife : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleColorOverLife, UParticleModule)

public:
    UParticleModuleColorOverLife();
    virtual ~UParticleModuleColorOverLife() override = default;

    // Color over life : When particle is update
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionVector, ColorOverLife)

    // Alpha over life : When particle is update
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionFloat, AlphaOverLife)

public:
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    virtual void DisplayProperty() override;
};
