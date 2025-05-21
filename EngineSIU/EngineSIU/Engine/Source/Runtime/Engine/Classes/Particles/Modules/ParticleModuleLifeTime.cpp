#include "ParticleModuleLifeTime.h"

#include "ParticleHelper.h"

UParticleModuleLifeTime::UParticleModuleLifeTime()
{
    bSpawnModule = true;
    bUpdateModule = false;
    
    LifeSpan.MinValue = 0.0f;
    LifeSpan.MaxValue = 1.0f;

    ModuleName = "LifeTime";
}

void UParticleModuleLifeTime::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    float Lifetime = FMath::Max(0.0f, LifeSpan.GetValue());

    if (Lifetime > KINDA_SMALL_NUMBER)
    {
        ParticleBase->OneOverMaxLifetime = 1.0f / Lifetime;
    }
    else
    {
        ParticleBase->OneOverMaxLifetime = 0.0f;
    }
    
    ParticleBase->RelativeTime = 0.0f;
}

void UParticleModuleLifeTime::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {  
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}
