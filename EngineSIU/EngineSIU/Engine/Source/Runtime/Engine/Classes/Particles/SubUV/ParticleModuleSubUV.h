#pragma once
#include "ParticleHelper.h"
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

    virtual void DisplayProperty() override;

    virtual int32 GetModulePayloadSize() const override
    {
        // 초기 속도 저장할 페이로드
        return sizeof(FFullSubUVPayload);
    }

    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase) override;
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    virtual int DetermineImageIndex(
        FParticleEmitterInstance* Owner, int32 Offset, FBaseParticle* Particle, bool bRandomMode, FFullSubUVPayload&
        SubUVPayload, float
        DeltaTime
    );

private:
    FFullSubUVPayload FullSubUVPayload;
};
