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
    UParticleLODLevel();
    virtual ~UParticleLODLevel() override = default;

    int32 Level = 0;

    bool bEnabled = true;

    UParticleModuleRequired* RequiredModule = nullptr;

    UParticleModuleSpawn* SpawnModule = nullptr;
    
    int32 PeakActiveParticles = 0;

    virtual int32 CalculateMaxActiveParticleCount();

    TArray<UParticleModule*> GetModules() const { return Modules;}
    void AddModule(UParticleModule* NewModule) { Modules.Add(NewModule); }
    void DeleteModule(UParticleModule* DeleteModule) { Modules.Remove(DeleteModule); }

private:
    TArray<UParticleModule*> Modules;
};
