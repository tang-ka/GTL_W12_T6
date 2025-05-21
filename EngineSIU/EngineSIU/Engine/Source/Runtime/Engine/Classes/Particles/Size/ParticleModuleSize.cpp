#include "ParticleModuleSize.h"
#include "Engine/ParticleEmitterInstance.h"
#include "Particles/ParticleSystemComponent.h"

UParticleModuleSize::UParticleModuleSize()
{
    bSpawnModule = true;
    bUpdateModule = false;

    bUseUniformSize = true;

    StartSize.MinValue = FVector::OneVector;
    StartSize.MaxValue = FVector::OneVector;

    ModuleName = "Size";
}

void UParticleModuleSize::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}

void UParticleModuleSize::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    FVector InitialSize;

    if (bUseUniformSize)
    {
        // UI에서 동일한 값으로 보이도록 설정 (X값을 기준으로 세팅)
        FVector Min = StartSize.MinValue;
        FVector Max = StartSize.MaxValue;

        float MinValue = Min.X;
        float MaxValue = Max.X;

        StartSize.MinValue = FVector(MinValue, MinValue, MinValue);
        StartSize.MaxValue = FVector(MaxValue, MaxValue, MaxValue);
        /////////////////////////////////////////////

        InitialSize = StartSize.GetValue();
        InitialSize = FVector(InitialSize.X, InitialSize.X, InitialSize.X);
    }
    else
    {
        InitialSize = StartSize.GetValue();
    }

    ParticleBase->BaseSize = InitialSize;
}
