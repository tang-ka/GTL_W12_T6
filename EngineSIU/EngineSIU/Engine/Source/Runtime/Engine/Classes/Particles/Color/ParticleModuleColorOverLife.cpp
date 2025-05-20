#include "ParticleModuleColorOverLife.h"

#include "ParticleEmitterInstance.h"
#include "ParticleHelper.h"

UParticleModuleColorOverLife::UParticleModuleColorOverLife()
{
    bSpawnModule = true;
    bUpdateModule = true;

    ColorOverLife.MinValue = FVector::ZeroVector;
    ColorOverLife.MaxValue = FVector::OneVector;

    AlphaOverLife.MinValue = 0.0f;
    AlphaOverLife.MaxValue = 1.0f;
}


void UParticleModuleColorOverLife::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    ParticleBase->Color = FLinearColor(ColorOverLife.MinValue, AlphaOverLife.MinValue);
}

void UParticleModuleColorOverLife::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    if (!Owner || Owner->ActiveParticles == 0 || Owner->ParticleData == nullptr)
    {
        return;
    }

    uint8* CurrentParticleData = Owner->ParticleData;
    const int32 ParticleStride = Owner->ParticleStride;
    FLinearColor Color = FLinearColor(ColorOverLife.MaxValue, AlphaOverLife.MaxValue);

    for (int32 i = 0; i < Owner->ActiveParticles; ++i)
    {
        FBaseParticle* Particle = (FBaseParticle*)(CurrentParticleData + ParticleStride * i);

        float RelativeTime = Particle->RelativeTime;
        // Setup color
        Particle->Color.Lerp(Particle->Color, Color, RelativeTime);
    }
}

void UParticleModuleColorOverLife::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {  
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}
