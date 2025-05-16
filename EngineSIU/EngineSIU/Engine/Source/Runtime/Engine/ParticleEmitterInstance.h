#pragma once
#include "HAL/PlatformType.h"
#include "ParticleHelper.h"

class UParticleLODLevel;
class UParticleSystemComponent;
class UParticleEmitter;

struct FParticleEmitterInstance
{
    /** The template this instance is based on.							*/
    UParticleEmitter* SpriteTemplate;
    /** The component who owns it.										*/
    UParticleSystemComponent* Component;
    /** The currently set LOD level.									*/
    UParticleLODLevel* CurrentLODLevel;
    /** The index of the currently set LOD level.						*/
    int32 CurrentLODLevelIndex;

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
    /** The maximum number of active particles that can be held in 
     *  the particle data array.
     */
    int32 MaxActiveParticles;
    /** The fraction of time left over from spawning.                   */

    void SpawnParticles(int32 Count, float StartTime, float Increment, const FVector& InitialLocation, const FVector& InitialVelocity);

    void KillParticle(int32 Index);
};

struct FParticleSpriteEmitterInstance : public FParticleEmitterInstance
{
    
};


struct FParticleMeshEmitterInstance : public FParticleEmitterInstance
{
    
};
