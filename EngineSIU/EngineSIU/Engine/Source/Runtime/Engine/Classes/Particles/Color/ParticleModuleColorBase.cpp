#include "ParticleModuleColorBase.h"
#include "ParticleEmitterInstance.h"

UParticleModuleColorBase::UParticleModuleColorBase()
{
    bSpawnModule = true;
    bUpdateModule = false;

    StartColor.MinValue = FVector::ZeroVector;
    StartColor.MaxValue = FVector::OneVector;

    StartAlpha.MinValue = 0.0f;
    StartAlpha.MaxValue = 1.0f;

    ModuleName = "Color";
}

void UParticleModuleColorBase::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    ParticleBase->Color = FLinearColor(StartColor.GetValue(), StartAlpha.GetValue());
}

void UParticleModuleColorBase::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {  
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}
