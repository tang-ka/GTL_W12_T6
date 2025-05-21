#include "ParticleLODLevel.h"

#include "UObject/Casts.h"
#include "Spawn/ParticleModuleSpawn.h"
#include "ParticleModuleRequired.h"
#include "Color/ParticleModuleColorBase.h"
#include "Color/ParticleModuleColorOverLife.h"
#include "Size/ParticleModuleSize.h"
#include "Location/ParticleModuleLocation.h"
#include "Modules/ParticleModuleLifeTime.h"
#include "Velocity/ParticleModuleVelocity.h"

UParticleLODLevel::UParticleLODLevel()
{
    RequiredModule = new UParticleModuleRequired();
    RequiredModule->ModuleName = "RequiredModule";
    Modules.Add(RequiredModule);

    SpawnModule = new UParticleModuleSpawn();
    SpawnModule->ModuleName = "SpawnModule";
    Modules.Add(SpawnModule);

    UParticleModuleSize* InitialScaleModule = new UParticleModuleSize();
    InitialScaleModule->ModuleName = "InitialScaleModule";
    Modules.Add(InitialScaleModule);

    UParticleModuleLocation* LocationModule = new UParticleModuleLocation();
    LocationModule->ModuleName = "LocationModule";
    Modules.Add(LocationModule);

    UParticleModuleVelocity* VelocityModule = new UParticleModuleVelocity();
    VelocityModule->ModuleName = "VelocityModule";
    Modules.Add(VelocityModule);

    UParticleModuleLifeTime* LifeTimeModule = new UParticleModuleLifeTime();
    LifeTimeModule->ModuleName = "LifeTimeModule";
    Modules.Add(LifeTimeModule);
    
    UParticleModuleColorBase* ColorModule = new UParticleModuleColorBase();
    ColorModule->ModuleName = "ColorModule";
    Modules.Add(ColorModule);

    // UParticleModuleColorOverLife* ColorOverLifeModule = new UParticleModuleColorOverLife();
    // ColorOverLifeModule->ModuleName = "ColorOverLifeModule";
    // Modules.Add(ColorOverLifeModule);
}

int32 UParticleLODLevel::CalculateMaxActiveParticleCount()
{
    // Determine the lifetime for particles coming from the emitter
	float ParticleLifetime = 0.0f;
    float MinSpawnRate;
	float MaxSpawnRate;
    SpawnModule->GetSpawnRate(MinSpawnRate, MaxSpawnRate);
	// int32 MaxBurstCount = SpawnModule->GetMaximumBurstCount();
	for (int32 ModuleIndex = 0; ModuleIndex < Modules.Num(); ModuleIndex++)
	{
		// UParticleModuleLifetimeBase* LifetimeMod = Cast<UParticleModuleLifetimeBase>(Modules[ModuleIndex]);
		// if (LifetimeMod != NULL)
		// {
		// 	ParticleLifetime += LifetimeMod->GetMaxLifetime();
		// }

		UParticleModuleSpawnBase* SpawnMod = Cast<UParticleModuleSpawnBase>(Modules[ModuleIndex]);
		if (SpawnMod != NULL)
		{
			// MaxSpawnRate += SpawnMod->GetMinimumSpawnRate();
		    MaxSpawnRate += MinSpawnRate;
			// MaxBurstCount += SpawnMod->GetMaximumBurstCount();
		}
	}

	// Determine the maximum duration for this particle system
	float MaxDuration = 0.0f;
	float TotalDuration = 0.0f;
	int32 TotalLoops = 0;
	if (RequiredModule != NULL)
	{
		// We don't care about delay wrt spawning...
		// MaxDuration = FMath::Max<float>(RequiredModule->EmitterDuration, RequiredModule->EmitterDurationLow); // TODO: 주석 풀기
		// TotalLoops = RequiredModule->EmitterLoops;
		TotalDuration = MaxDuration * TotalLoops;
	}

	// Determine the max
	int32 MaxAPC = 0;

	if (TotalDuration != 0.0f)
	{
		if (TotalLoops == 1)
		{
			// Special case for one loop... 
			if (ParticleLifetime < MaxDuration)
			{
				MaxAPC += FMath::CeilToInt(ParticleLifetime * MaxSpawnRate);
			}
			else
			{
				MaxAPC += FMath::CeilToInt(MaxDuration * MaxSpawnRate);
			}
			// Safety zone...
			MaxAPC += 1;
			// Add in the bursts...
			// MaxAPC += MaxBurstCount;
		}
		else
		{
			if (ParticleLifetime < MaxDuration)
			{
				MaxAPC += FMath::CeilToInt(ParticleLifetime * MaxSpawnRate);
			}
			else
			{
				MaxAPC += (FMath::CeilToInt(FMath::CeilToInt(MaxDuration * MaxSpawnRate) * ParticleLifetime));
			}
			// Safety zone...
			MaxAPC += 1;
			// Add in the bursts...
			// MaxAPC += MaxBurstCount;
			if (ParticleLifetime > MaxDuration)
			{
				// MaxAPC += MaxBurstCount * FMath::CeilToInt(ParticleLifetime - MaxDuration);
			}
		}
	}
	else
	{
		// We are infinite looping... 
		// Single loop case is all we will worry about. Safer base estimate - but not ideal.
		if (ParticleLifetime < MaxDuration)
		{
			MaxAPC += FMath::CeilToInt(ParticleLifetime * FMath::CeilToInt(MaxSpawnRate));
		}
		else
		{
			if (ParticleLifetime != 0.0f)
			{
				if (ParticleLifetime <= MaxDuration)
				{
					MaxAPC += FMath::CeilToInt(MaxDuration * MaxSpawnRate);
				}
				else //if (ParticleLifetime > MaxDuration)
				{
					MaxAPC += FMath::CeilToInt(MaxDuration * MaxSpawnRate) * ParticleLifetime;
				}
			}
			else
			{
				// No lifetime, no duration...
				MaxAPC += FMath::CeilToInt(MaxSpawnRate);
			}
		}
		// Safety zone...
		MaxAPC += FMath::Max<int32>(FMath::CeilToInt(MaxSpawnRate * 0.032f), 2);
		// Burst
		// MaxAPC += MaxBurstCount;
	}

	PeakActiveParticles = MaxAPC;

	return MaxAPC;
}
