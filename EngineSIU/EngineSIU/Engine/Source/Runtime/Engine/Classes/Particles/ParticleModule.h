#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FBaseParticle;
struct FParticleEmitterInstance;

enum class EModuleType : uint8
{
    /** General - all emitter types can use it			*/
    EPMT_General,
    /** TypeData - TypeData modules						*/
    EPMT_TypeData,
    /** Beam - only applied to beam emitters			*/
    EPMT_Beam,
    /** Trail - only applied to trail emitters			*/
    EPMT_Trail,
    /** Spawn - all emitter types REQUIRE it			*/
    EPMT_Spawn,
    /** Required - all emitter types REQUIRE it			*/
    EPMT_Required,
    /** Event - event related modules					*/
    EPMT_Event,
    /** Light related modules							*/
    EPMT_Light,
    /** SubUV related modules							*/
    EPMT_SubUV,
    EPMT_MAX,
};

class UParticleModule : public UObject
{
    DECLARE_CLASS(UParticleModule, UObject)

public:
    UParticleModule() = default;
    virtual ~UParticleModule() override = default;

    virtual void DisplayProperty() override {};

    FName ModuleName;
    
    bool bSpawnModule = false;
    bool bUpdateModule = false;
    bool bFinalUpdateModule = false;
    
    bool bEnabled = true;

    int32 ModulePayloadOffset = 0;
    int32 InstancePayloadOffset = 0;
    /**
     *	Called on a particle that is freshly spawned by the emitter.
     *	
     *	@param	Owner		The FParticleEmitterInstance that spawned the particle.
     *	@param	Offset		The modules offset into the data payload of the particle.
     *	@param	SpawnTime	The time of the spawn.
     */
    virtual void Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase);
    /**
     *	Called on a particle that is being updated by its emitter.
     *	
     *	@param	Owner		The FParticleEmitterInstance that 'owns' the particle.
     *	@param	Offset		The modules offset into the data payload of the particle.
     *	@param	DeltaTime	The time since the last update.
     */
    virtual void Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    /**
     *	Called on an emitter when all other update operations have taken place
     *	INCLUDING bounding box cacluations!
     *	
     *	@param	Owner		The FParticleEmitterInstance that 'owns' the particle.
     *	@param	Offset		The modules offset into the data payload of the particle.
     *	@param	DeltaTime	The time since the last update.
     */
    virtual void FinalUpdate(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime);
    
    virtual EModuleType	GetModuleType() const { return EModuleType::EPMT_General; }

    virtual int32 GetModulePayloadSize() const;
    void SetModulePayloadOffset(int32 Offset) { ModulePayloadOffset = Offset; }

    virtual int32 GetInstancePayloadSize() const;
    void SetInstancePayloadOffset(int32 Size) { InstancePayloadOffset = Size; }
};
