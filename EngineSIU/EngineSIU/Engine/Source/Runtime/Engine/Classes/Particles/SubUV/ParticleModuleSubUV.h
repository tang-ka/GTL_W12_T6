#pragma once
#include "ParticleModuleSUbUVBase.h"
#include "Distribution/DistributionFloat.h"
#include "Particles/ParticleEmitter.h"

struct FFullSubUVPayload;

class UParticleModuleSubUV : public UParticleModuleSUbUVBase
{
    DECLARE_CLASS(UParticleModuleSubUV, UParticleModuleSUbUVBase)
public:
    UParticleModuleSubUV();

    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionFloat, SubImageIndex)

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    virtual int DetermineImageIndex(
        FParticleEmitterInstance* Owner, int32 Offset, FBaseParticle* Particle, bool bRandomMode, FFullSubUVPayload
        & SubUVPayload, float DeltaTime
    );
};
