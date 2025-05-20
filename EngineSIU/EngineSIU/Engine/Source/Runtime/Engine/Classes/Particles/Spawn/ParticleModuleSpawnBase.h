#pragma once
#include "Particles/ParticleModule.h"
#include "UObject/ObjectMacros.h"


class UParticleModuleSpawnBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleSpawnBase, UParticleModule)

public:
    UParticleModuleSpawnBase() = default;
    virtual ~UParticleModuleSpawnBase() override = default;

    virtual void DisplayProperty() override;
public:

	/** 
	 *	If true, the SpawnRate of the SpawnModule of the emitter will be processed.
	 *	If mutliple Spawn modules are 'stacked' in an emitter, if ANY of them 
	 *	have this set to false, it will not process the SpawnModule SpawnRate.
	 */
	UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bProcessSpawnRate)

	/** 
	 *	If true, the BurstList of the SpawnModule of the emitter will be processed.
	 *	If mutliple Spawn modules are 'stacked' in an emitter, if ANY of them 
	 *	have this set to false, it will not process the SpawnModule BurstList.
	 */
	UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bProcessBurstList)
    
	//~ Begin UParticleModule Interface
	virtual EModuleType	GetModuleType() const override {	return EModuleType::EPMT_Spawn;	}
	//~ End UParticleModule Interface

	/**
	 *	Retrieve the spawn amount this module is contributing.
	 *	Note that if multiple Spawn-specific modules are present, if any one
	 *	of them ignores the SpawnRate processing it will be ignored.
	 *
	 *	@param	Owner		The particle emitter instance that is spawning.
	 *	@param	Offset		The offset into the particle payload for the module.
	 *	@param	OldLeftover	The bit of timeslice left over from the previous frame.
	 *	@param	DeltaTime	The time that has expired since the last frame.
	 *	@param	Number		The number of particles to spawn. (OUTPUT)
	 *	@param	Rate		The spawn rate of the module. (OUTPUT)
	 *
	 *	@return	bool		false if the SpawnRate should be ignored.
	 *						true if the SpawnRate should still be processed.
	 */
	virtual bool GetSpawnAmount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover,  float DeltaTime, int32& Number, float& Rate)
	{
		return bProcessSpawnRate;
	}
	
	/**
	 *	Retrieve the burst count this module is contributing.
	 *	Note that if multiple Spawn-specific modules are present, if any one
	 *	of them ignores the default BurstList, it will be ignored.
	 *
	 *	@param	Owner		The particle emitter instance that is spawning.
	 *	@param	Offset		The offset into the particle payload for the module.
	 *	@param	OldLeftover	The bit of timeslice left over from the previous frame.
	 *	@param	DeltaTime	The time that has expired since the last frame.
	 *	@param	Number		The number of particles to burst. (OUTPUT)
	 *
	 *	@return	bool		false if the default BurstList should be ignored.
	 *						true if the default BurstList should still be processed.
	 */
	virtual bool GetBurstCount(FParticleEmitterInstance* Owner, int32 Offset, float OldLeftover, float DeltaTime, int32& Number)
	{
		Number = 0;
		return bProcessBurstList;
	}

	/**
	 *	Retrieve the maximum spawn rate for this module...
	 *	Used in estimating the number of particles that could be used.
	 *
	 *	@return	float		The maximum spawn rate
	 */
	virtual void GetSpawnRate(float& MinSpawnRate, float& MaxSpawnRate) { }
	//
	// /**
	//  *	Retrieve the estimated spawn rate for this module...
	//  *	Used in estimating the number of particles that could be used.
	//  *
	//  *	@return	float			The maximum spawn rate
	//  */
	// virtual float GetMinimumSpawnRate() { return 0.0f; }
	//
	// /**
	//  *	Retrieve the maximum number of particles this module could burst.
	//  *	Used in estimating the number of particles that could be used.
	//  *
	//  *	@return	int32			The maximum burst count
	//  */
	// virtual int32 GetMaximumBurstCount() { return 0; }
};
