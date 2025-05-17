#pragma once
#include "ParticleModuleSpawnBase.h"
#include "UObject/ObjectMacros.h"

enum class EParticleBurstMethod : uint8;

class UParticleModuleSpawn : public UParticleModuleSpawnBase
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModuleSpawnBase)

public:
    UParticleModuleSpawn() = default;
    virtual ~UParticleModuleSpawn() override = default;

    /** The rate at which to spawn particles. */
    // FRawDistributionFloat Rate;
    float Rate; // 일단 상수 값으로 진행
    

    /** The scalar to apply to the rate. */
    // FRawDistributionFloat RateScale;
    float RateScale; // 일단 상수 값으로 진행

    /**	If true, the SpawnRate will be scaled by the global CVar r.EmitterSpawnRateScale */
    uint32 bApplyGlobalSpawnRateScale : 1;

    //~ Begin UParticleModuleSpawnBase Interface
    virtual bool GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover, float DeltaTime, int32& Number, float& Rate) override;
    virtual float GetMaximumSpawnRate() override;
    virtual float GetEstimatedSpawnRate() override;
    //~ End UParticleModuleSpawnBase Interface
};
