#include "ParticleEmitterInstance.h"

void FParticleEmitterInstance::SpawnParticles(
    int32 Count, float StartTime, float Increment,
    const FVector& InitialLocation, const FVector& InitialVelocity)
{
    for (int32 i = 0; i < Count; i++)
    {
        DECLARE_PARTICLE_PTR(Particle, ParticleData + ParticleStride * NextFreeIndex)
        PreSpawn(Particle, InitialLocation, InitialVelocity);

        for (int32 ModuleIndex = 0; ModuleIndex < LODLevel->SpawnModules.Num(); ModuleIndex++)
        {
            
        }

        PostSpawn(Particle, Interp, SpawnTime);
    }
}

void FParticleEmitterInstance::KillParticle(int32 Index)
{
}
