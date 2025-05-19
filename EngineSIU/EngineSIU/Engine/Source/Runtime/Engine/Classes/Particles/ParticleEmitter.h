#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleLODLevel;

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
    UParticleEmitter();
    virtual ~UParticleEmitter() override = default;

    void CacheEmitterModuleInfo();

    TArray<UParticleLODLevel*> GetLODLevels() const { return LODLevels; }
    UParticleLODLevel* GetLODLevel(int32 LODIndex) const;

public:
    FName EmitterName;
    int32 PeakActiveParticles = 0;

    // Below is information udpated by calling CacheEmitterModuleInfo
    
    int32 ParticleSize;

private:
    TArray<UParticleLODLevel*> LODLevels; // 현재는 Level 0만 사용
};
