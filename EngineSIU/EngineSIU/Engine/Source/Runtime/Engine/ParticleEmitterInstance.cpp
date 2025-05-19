#include "ParticleEmitterInstance.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModule.h"
#include "Particles/ParticleEmitter.h"
//#include "Particles/ParticleModuleRequired.h"

void FParticleEmitterInstance::Initialize()
{
    const TArray<UParticleModule*>& Modules = CurrentLODLevel->Modules;

    CurrentLODLevel = SpriteTemplate->GetLODLevel(CurrentLODLevelIndex);

    if (CurrentLODLevel->RequiredModule)
    {
        MaxActiveParticles = CurrentLODLevel->CalculateMaxActiveParticleCount();
    }
    else
    {
        MaxActiveParticles = 1000;
    }

    PayloadOffset = sizeof(FBaseParticle);

    ParticleSize = PayloadOffset;
    InstancePayloadSize = 0;
    for (auto* Module : Modules)
    {
        if (Module->bSpawnModule)
        {
            int32 ModulePayloadSize = Module->GetModulePayloadOffset();
            Module->SetModulePayloadOffset(PayloadOffset);
            ParticleSize += ModulePayloadSize;
        }

        if (Module->bUpdateModule)
        {
            int32 Size = Module->GetInstancePayloadOffset();
            Module->SetInstancePayloadOffset(InstancePayloadSize);
            InstancePayloadSize += Size;
        }
    }

    ParticleStride = ParticleSize;

    ParticleData = new uint8[MaxActiveParticles * ParticleStride];
    ParticleIndices = new uint16[MaxActiveParticles];
    InstanceData = new uint8[MaxActiveParticles * InstancePayloadSize];

    ActiveParticles = 0;
    ParticleCounter = 0;
    AccumulatedTime = 0.0f;
    SpawnFraction = 0.0f;
}

void FParticleEmitterInstance::Tick(float DeltaTime)
{
    AccumulatedTime += DeltaTime;

    // 초당 생성속도에 따른 현재 프레임에 생성할 파티클 수 계산
    int32 SpawnCount = CalculateSpawnCount(DeltaTime);
    // 최대 활성 파티클 수를 초과하지 않도록 조정
    int32 AvailableParticles = MaxActiveParticles - ActiveParticles;
    SpawnCount = FMath::Min(SpawnCount, AvailableParticles);

    if (SpawnCount > 0)
    {
        float Increment = (SpawnCount > 1) ? DeltaTime / (SpawnCount - 1) : 0.0f;
        float StartTime = AccumulatedTime - DeltaTime;
        SpawnParticles(SpawnCount, StartTime, Increment, FVector::ZeroVector, FVector::ZeroVector);
    }

    UpdateModules(DeltaTime);
    UpdateParticles(DeltaTime);
}

void FParticleEmitterInstance::SpawnParticles(
    int32 Count, float StartTime, float Increment,
    const FVector& InitialLocation, const FVector& InitialVelocity)
{
    for (int32 i = 0; i < Count; i++)
    {
        int32 NextIndex = ActiveParticles++;
        DECLARE_PARTICLE_PTR(Particle, ParticleData + ParticleStride * NextIndex)

        PreSpawn(Particle, InitialLocation, InitialVelocity);

        int32 Offset = 0;
        float SpawnTime = StartTime + Increment * i;

        for (auto* Module : CurrentLODLevel->Modules)
        {
            Module->Spawn(this, Offset, SpawnTime, Particle);
        }

        float Interp = (Count > 1) ? (float)i / (float)(Count - 1) : 0.f;
        PostSpawn(Particle, Interp, SpawnTime);
    }
}

void FParticleEmitterInstance::KillParticle(int32 Index)
{
    if (Index < 0 || Index >= ActiveParticles)
    {
        return;
    }

    //int32 LastIndex = ActiveParticles - 1;
    //if (Index != LastIndex)
    //{
    //    FMemory::Memcpy(
    //        ParticleData + Index * ParticleStride,
    //        ParticleData + LastIndex * ParticleStride,
    //        ParticleStride);
    //}
    ActiveParticles--;
}

int32 FParticleEmitterInstance::CalculateSpawnCount(float DeltaTime)
{
    //float Rate = CurrentLODLevel->RequiredModule->SpawnRateDistribution->GetValue();
    float Rate = 10.0f;

    float NewParticlesFloat = Rate * DeltaTime + SpawnFraction;
    int32 SpawnCount = FMath::FloorToInt(NewParticlesFloat);
    SpawnFraction = NewParticlesFloat - SpawnCount;

    return SpawnCount;
}

void FParticleEmitterInstance::PreSpawn(FBaseParticle* Particle, const FVector& InitialLocation, const FVector& InitialVelocity)
{
    Particle->OldLocation = InitialLocation;
    Particle->Location = InitialLocation;

    Particle->BaseVelocity = InitialVelocity;
    Particle->Velocity = InitialVelocity;

    Particle->BaseRotationRate = 0.f;
    Particle->Rotation = 0.f;
    Particle->RotationRate = 0.f;

    Particle->BaseSize = FVector(1.f, 1.f, 1.f);
    Particle->Size = Particle->BaseSize;
    Particle->Flags = 0;

    Particle->Color = Particle->BaseColor;

    Particle->BaseColor = FLinearColor::White;

    Particle->RelativeTime = 0.f;
    Particle->OneOverMaxLifetime = 1.f; // 나중에 수명 기반 분포에서 설정 가능
    Particle->Placeholder0 = 0.f;
    Particle->Placeholder1 = 0.f;

}

void FParticleEmitterInstance::PostSpawn(FBaseParticle* Particle, float Interp, float SpawnTime)
{
    Particle->Rotation = 0.f;
    Particle->Color = Particle->BaseColor;
}

void FParticleEmitterInstance::UpdateModules(float DeltaTime)
{
    for (auto* Module : CurrentLODLevel->Modules)
    {
        if (Module->bUpdateModule)
        {
            int32 Offset = Module->GetInstancePayloadOffset();
            Module->Update(this, Offset, DeltaTime);
        }
    }
}

void FParticleEmitterInstance::UpdateParticles(float DeltaTime)
{
    for (int32 i = 0; i < ActiveParticles; ++i)
    {
        DECLARE_PARTICLE_PTR(Particle, ParticleData + ParticleStride * i);

        // 상대 시간 업데이트
        Particle->RelativeTime += DeltaTime * Particle->OneOverMaxLifetime;

        // 수명 종료 판단
        //if (Particle->RelativeTime >= 1.0f)
        //{
        //    KillParticle(i);
        //    --i; // 마지막 인덱스를 앞으로 복사했으므로 인덱스 유지
        //    continue;
        //}

        // 위치 업데이트
        Particle->OldLocation = Particle->Location;
        Particle->Velocity = Particle->BaseVelocity;
        Particle->Location += Particle->Velocity * DeltaTime;

        // 회전 업데이트
        Particle->RotationRate = Particle->BaseRotationRate;
        Particle->Rotation += Particle->RotationRate * DeltaTime;

        // 크기 업데이트
        Particle->Size = Particle->BaseSize;
    }
}
