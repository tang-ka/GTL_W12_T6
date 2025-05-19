#include "ParticleModuleSpawn.h"

void UParticleModuleSpawn::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}

bool UParticleModuleSpawn::GetSpawnAmount(FParticleEmitterInstance* Owner,
                                          int32 Offset, float OldLeftover, float DeltaTime, int32& Number, float& InRate)
{
    return false;
}

float UParticleModuleSpawn::GetMaximumSpawnRate()
{
    return Rate * RateScale;
    
    float MinSpawn, MaxSpawn;
    float MinScale, MaxScale;

    //Rate.GetOutRange(MinSpawn, MaxSpawn);
    //RateScale.GetOutRange(MinScale, MaxScale);

    return (MaxSpawn * MaxScale);
}

float UParticleModuleSpawn::GetEstimatedSpawnRate()
{
    return Rate * RateScale;
    
    float MinSpawn, MaxSpawn;
	float MinScale, MaxScale;

	//Rate.GetOutRange(MinSpawn, MaxSpawn);
	//RateScale.GetOutRange(MinScale, MaxScale);

	return (MaxSpawn * MaxScale);
}
