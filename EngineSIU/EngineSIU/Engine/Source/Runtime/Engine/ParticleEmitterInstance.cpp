#include "ParticleEmitterInstance.h"
#include "ParticleLODLevel.h"

void FParticleEmitterInstance::Tick(float DeltaTime)
{
}

void FParticleEmitterInstance::SpawnParticles(
    int32 Count, float StartTime, float Increment,
    const FVector& InitialLocation, const FVector& InitialVelocity)
{
    for (int32 i = 0; i < Count; i++)
    {
        DECLARE_PARTICLE_PTR(Particle, ParticleData + ParticleStride * NextFreeIndex)
        PreSpawn(Particle, InitialLocation, InitialVelocity);

        for (int32 ModuleIndex = 0; ModuleIndex < CurrentLODLevel->Modules.Num(); ModuleIndex++)
        {
            
        }

        PostSpawn(Particle, Interp, SpawnTime);
    }
}

void FParticleEmitterInstance::KillParticle(int32 Index)
{
}

void FParticleEmitterInstance::PreSpawn(FBaseParticle* Particle, const FVector& InitialLocation, const FVector& InitialVelocity)
{
}

void FParticleEmitterInstance::PostSpawn(FBaseParticle* Particle, float Interp, float SpawnTime)
{
}

void FParticleEmitterInstance::UpdateParticles(float DeltaTime)
{
}

void FParticleEmitterInstance::UpdateModules(float DeltaTime)
{
}
