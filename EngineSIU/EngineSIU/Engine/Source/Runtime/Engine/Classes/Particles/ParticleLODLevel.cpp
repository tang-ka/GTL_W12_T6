#include "ParticleLODLevel.h"

#include "UObject/Casts.h"
#include "Spawn/ParticleModuleSpawn.h"
#include "ParticleModuleRequired.h"
#include "Color/ParticleModuleColorBase.h"
#include "Color/ParticleModuleColorOverLife.h"
#include "Size/ParticleModuleSize.h"
#include "UObject/ObjectFactory.h"
#include "Location/ParticleModuleLocation.h"
#include "Modules/ParticleModuleLifeTime.h"
#include "SubUV/ParticleModuleSubUV.h"
#include "Velocity/ParticleModuleVelocity.h"
#include "Velocity/ParticleModuleVelocityOverLife.h"

UParticleLODLevel::UParticleLODLevel()
{
    RequiredModule = FObjectFactory::ConstructObject<UParticleModuleRequired>(this);
    Modules.Add(RequiredModule);

    SpawnModule = FObjectFactory::ConstructObject<UParticleModuleSpawn>(this);
    Modules.Add(SpawnModule);

    UParticleModuleSize* InitialScaleModule = FObjectFactory::ConstructObject<UParticleModuleSize>(this);
    Modules.Add(InitialScaleModule);

    UParticleModuleLocation* LocationModule = FObjectFactory::ConstructObject<UParticleModuleLocation>(this);
    Modules.Add(LocationModule);

    UParticleModuleVelocity* VelocityModule = FObjectFactory::ConstructObject<UParticleModuleVelocity>(this);
    Modules.Add(VelocityModule);

    UParticleModuleLifeTime* LifeTimeModule = FObjectFactory::ConstructObject<UParticleModuleLifeTime>(this);
    Modules.Add(LifeTimeModule);
    
    UParticleModuleColorBase* ColorModule = FObjectFactory::ConstructObject<UParticleModuleColorBase>(this);
    Modules.Add(ColorModule);

    UParticleModuleColorOverLife* ColorOverLifeModule = FObjectFactory::ConstructObject<UParticleModuleColorOverLife>(this);
    Modules.Add(ColorOverLifeModule);

    UParticleModuleVelocityOverLife* VelocityOverLifeModule = FObjectFactory::ConstructObject<UParticleModuleVelocityOverLife>(this);
    Modules.Add(VelocityOverLifeModule);

    UParticleModuleSubUV* SubUVModule = FObjectFactory::ConstructObject<UParticleModuleSubUV>(this);
    Modules.Add(SubUVModule);
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
