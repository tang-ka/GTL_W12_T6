#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

enum class EParticleBurstMethod : uint8
{
    EPBM_Instant,
    EPBM_Interpolated,
    EPBM_MAX,
};

class UParticleEmitter : public UObject
{
    DECLARE_CLASS(UParticleEmitter, UObject)

public:
    UParticleEmitter() = default;
    virtual ~UParticleEmitter() override = default;
    
};
