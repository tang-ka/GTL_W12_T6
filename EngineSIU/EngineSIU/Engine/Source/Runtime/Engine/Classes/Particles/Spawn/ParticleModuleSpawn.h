#pragma once
#include "ParticleModuleSpawnBase.h"
#include "Distribution/DistributionFloat.h"
#include "UObject/ObjectMacros.h"

enum class EParticleBurstMethod : uint8;

class UParticleModuleSpawn : public UParticleModuleSpawnBase
{
    DECLARE_CLASS(UParticleModuleSpawn, UParticleModuleSpawnBase)

public:
    UParticleModuleSpawn();
    virtual ~UParticleModuleSpawn() override = default;

    virtual void DisplayProperty() override;

    /** The rate at which to spawn particles. */
    // FRawDistributionFloat Rate;
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionFloat, Rate) // 일단 상수 값으로 진행
    
    /** The scalar to apply to the rate. */
    // FRawDistributionFloat RateScale;
    UPROPERTY_WITH_FLAGS(EditAnywhere, FDistributionFloat, RateScale) // 일단 상수 값으로 진행

    UPROPERTY_WITH_FLAGS(EditAnywhere, float, BurstTime)
    UPROPERTY_WITH_FLAGS(EditAnywhere, int, BurstCount)

    // /**	If true, the SpawnRate will be scaled by the global CVar r.EmitterSpawnRateScale */
    // UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bApplyGlobalSpawnRateScale)

    //~ Begin UParticleModuleSpawnBase Interface
    virtual bool GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover, float DeltaTime, int32& Number, float& Rate) override;
    virtual void GetSpawnRate(float& MinSpawnRate, float& MaxSpawnRate) override;
    
    //~ End UParticleModuleSpawnBase Interface
};
