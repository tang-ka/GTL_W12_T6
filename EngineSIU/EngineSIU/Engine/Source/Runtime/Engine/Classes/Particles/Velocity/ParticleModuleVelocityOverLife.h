#pragma once
#include "ParticleModuleVelocityBase.h"

class UParticleModuleVelocityOverLife : public UParticleModuleVelocityBase
{
    DECLARE_CLASS(UParticleModuleVelocityOverLife, UParticleModuleVelocityBase)

public:
    UParticleModuleVelocityOverLife();
    virtual ~UParticleModuleVelocityOverLife() override = default;

    virtual void DisplayProperty() override;

    virtual int32 GetModulePayloadSize() const override
    {
        // 초기 속도 저장할 페이로드
        return sizeof(FVector);
    }

    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime) override;

    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseConstantChange)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseVelocityCurve)

    UPROPERTY_WITH_FLAGS(EditAnywhere, float, CurveScale)

    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, StartVelocity)
    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, EndVelocity)
};

