#include "ParticleModuleSpawn.h"

UParticleModuleSpawn::UParticleModuleSpawn()
{
    Rate.MinValue = 20.0f;
    Rate.MaxValue = 20.0f;

    BurstTime = 1.0f;
    BurstCount = 200;

    ModuleName = "Spawn"; 
}

void UParticleModuleSpawn::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}

bool UParticleModuleSpawn::GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset,
    float OldLeftover, float DeltaTime, int32& Number, float& InRate)
{
    return Owner != nullptr;
}

void UParticleModuleSpawn::GetSpawnRate(float& MinSpawnRate, float& MaxSpawnRate)
{
    float MinSpawn, MaxSpawn;
    float MinScale, MaxScale;

    Rate.GetOutRange(MinSpawn, MaxSpawn);
    RateScale.GetOutRange(MinScale, MaxScale);

    MinSpawnRate = MinSpawn * MinScale;
    MaxSpawnRate = MaxSpawn * MaxScale;
}
