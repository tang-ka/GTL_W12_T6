#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleModule;
class UParticleModuleRequired;
class UParticleModuleSpawn;

class UParticleLODLevel : public UObject
{
    DECLARE_CLASS(UParticleLODLevel, UObject)

public:
    UParticleLODLevel() = default;
    virtual ~UParticleLODLevel() override = default;

    int32 Level = 0;

    bool bEnabled = true;

    UParticleModuleRequired* RequiredModule = nullptr;

    UParticleModuleSpawn* SpawnModule = nullptr;

    TArray<UParticleModule*> Modules;

    int32 PeakActiveParticles = 0;

    virtual int32 CalculateMaxActiveParticleCount();
};
