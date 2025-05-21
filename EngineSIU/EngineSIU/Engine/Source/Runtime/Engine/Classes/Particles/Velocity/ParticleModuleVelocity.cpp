#include "ParticleModuleVelocity.h"
#include "ParticleHelper.h"
#include "Engine/ParticleEmitterInstance.h"
#include "Particles/ParticleSystemComponent.h"

UParticleModuleVelocity::UParticleModuleVelocity()
{
    bSpawnModule = true;
    bUpdateModule = false;

    ModuleName = "Velocity";
}

void UParticleModuleVelocity::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    FVector Velocity = StartVelocity.GetValue();
    float RadialStrength = StartVelocityRadial.GetValue();

    if (RadialStrength > 0.f)
    {
        FVector EmitterLocation = Owner->Component->GetComponentToWorld().GetTranslation();
        FVector Direction = ParticleBase->Location.GetSafeNormal();
        Velocity += Direction * RadialStrength;
    }

    if (bInWorldSpace)
    {
        Velocity = Owner->Component->GetComponentToWorld().TransformVector(Velocity);
    }

    if (bApplyOwnerScale)
    {
        Velocity *= Owner->Component->GetComponentToWorld().GetScale3D();
    }

    ParticleBase->BaseVelocity = Velocity;
}
