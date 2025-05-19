#pragma once
#include "HAL/PlatformType.h"
#include "ParticleHelper.h"

class UParticleLODLevel;
class UParticleSystemComponent;
class UParticleEmitter;
struct DistributionFloat;

struct FParticleEmitterInstance
{
public:
    UParticleEmitter* SpriteTemplate;

    UParticleSystemComponent* Component;

    int32 CurrentLODLevelIndex;
    UParticleLODLevel* CurrentLODLevel;

    /** Pointer to the particle data array.                             */
    uint8* ParticleData;
    /** Pointer to the particle index array.                            */
    uint16* ParticleIndices;
    /** Pointer to the instance data array.                             */
    uint8* InstanceData;
    /** The size of the Instance data array.                            */
    int32 InstancePayloadSize;
    /** The offset to the particle data.                                */
    int32 PayloadOffset;
    /** The total size of a particle (in bytes).                        */
    int32 ParticleSize;
    /** The stride between particles in the ParticleData array.         */
    int32 ParticleStride;
    /** The number of particles currently active in the emitter.        */
    int32 ActiveParticles;
    /** Monotonically increasing counter. */
    uint32 ParticleCounter;
    /** The maximum number of active particles that can be held in the particle data array.*/
    int32 MaxActiveParticles;
    /** The fraction of time left over from spawning.                   */

    float AccumulatedTime = 0;
    DistributionFloat* SpawnRateDistribution;
    float SpawnFraction = 0;

public:
    void Initialize();

    void Tick(float DeltaTime);
    void SpawnParticles(int32 Count, float StartTime, float Increment, 
                        const FVector& InitialLocation, const FVector& InitialVelocity);
    void KillParticle(int32 Index);

    int32 CalculateSpawnCount(float DeltaTime);

    void PreSpawn(FBaseParticle* Particle, const FVector& InitialLocation, const FVector& InitialVelocity);
    void PostSpawn(FBaseParticle* Particle, float Interp, float SpawnTime);
    void UpdateParticles(float DeltaTime);
    void UpdateModules(float DeltaTime);
};

struct FParticleSpriteEmitterInstance : public FParticleEmitterInstance
{
    
};


struct FParticleMeshEmitterInstance : public FParticleEmitterInstance
{
    
};
