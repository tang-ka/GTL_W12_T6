#include "ParticleModule.h"

void UParticleModule::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
}

void UParticleModule::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
}

void UParticleModule::FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
}

int32 UParticleModule::GetModulePayloadOffset() const
{
    return int32();
}

int32 UParticleModule::GetInstancePayloadOffset() const
{
    return int32();
}
