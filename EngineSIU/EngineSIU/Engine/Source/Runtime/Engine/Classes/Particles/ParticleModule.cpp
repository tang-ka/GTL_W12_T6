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

int32 UParticleModule::GetModulePayloadSize() const
{
    return int32();
}

int32 UParticleModule::GetInstancePayloadSize() const
{
    return int32();
}
