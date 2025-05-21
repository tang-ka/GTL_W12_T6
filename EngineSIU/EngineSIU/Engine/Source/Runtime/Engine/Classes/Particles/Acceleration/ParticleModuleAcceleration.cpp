#include "ParticleModuleAcceleration.h"
#include "Engine/ParticleHelper.h"
#include "Engine/ParticleEmitterInstance.h"
#include "Particles/ParticleSystemComponent.h"

UParticleModuleAcceleration::UParticleModuleAcceleration()
{
    ModuleName = "Acceleration";
    bSpawnModule = false;
    bUpdateModule = false;
    bFinalUpdateModule = true;

    // 기본 가속도 설정
    Acceleration = FVector(0.0f, 0.0f, -2.0f);
}

void UParticleModuleAcceleration::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}

void UParticleModuleAcceleration::FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP

        Particle.BaseVelocity += Acceleration * DeltaTime;

    END_UPDATE_LOOP
}
