#include "ParticleModuleSubUV.h"

#include "ParticleEmitterInstance.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleModuleRequired.h"

UParticleModuleSubUV::UParticleModuleSubUV()
{
    bSpawnModule = true;
    bUpdateModule = true;
    
    ModuleName = "SubUV";
}

void UParticleModuleSubUV::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
    if (!Owner->SpriteTemplate)
    {
        return;
    }

    UParticleLODLevel* LODLevel	= Owner->SpriteTemplate->GetLODLevel(0);

    if (!LODLevel)
    {
        return;
    }
    bool bRandomSubUV = LODLevel->RequiredModule->bSubUVRandomMode;
    
    DetermineImageIndex(Owner, Offset, ParticleBase, bRandomSubUV, FullSubUVPayload, SpawnTime);

    Owner->SubUVDataOffset = ModulePayloadOffset;
    
    // payload 위치 계산
    uint8* writableBase = reinterpret_cast<uint8*>(ParticleBase);
    FFullSubUVPayload* InitialVelPtr = reinterpret_cast<FFullSubUVPayload*>(writableBase + ModulePayloadOffset);
    *InitialVelPtr = FullSubUVPayload;
}

void UParticleModuleSubUV::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    if(!Owner->SpriteTemplate)
    {
        return;
    }

    UParticleLODLevel* LODLevel	= Owner->SpriteTemplate->GetLODLevel(0);

    bool bRandomSubUV = LODLevel->RequiredModule->bSubUVRandomMode;
    const int32 PayloadOffset = Owner->SubUVDataOffset;
    
    BEGIN_UPDATE_LOOP
    if (Particle.RelativeTime > 1.0f)
    {
        CONTINUE_UPDATE_LOOP
    }

    DetermineImageIndex(Owner, Offset, &Particle, bRandomSubUV, FullSubUVPayload, DeltaTime);

    uint8* writableBase = const_cast<uint8*>(ParticleBase);
    FFullSubUVPayload* InitialVelPtr = reinterpret_cast<FFullSubUVPayload*>(writableBase + ModulePayloadOffset);
    *InitialVelPtr = FullSubUVPayload;
    END_UPDATE_LOOP
}

int UParticleModuleSubUV::DetermineImageIndex(
    FParticleEmitterInstance* Owner, int32 Offset, FBaseParticle* Particle, bool bRandomMode, FFullSubUVPayload& SubUVPayload, float DeltaTime
)
{
    UParticleLODLevel* LODLevel	= Owner->SpriteTemplate->GetLODLevel(0);
    if (LODLevel == nullptr)
    {
        return -1;
    }
    
    const int32 TotalSubImages = LODLevel->RequiredModule->SubImagesHorizontal * LODLevel->RequiredModule->SubImagesVertical;

    int ImageIndex = SubUVPayload.ImageIndex;

    if (bRandomMode)
    {
        if ((LODLevel->RequiredModule->RandomImageTime == 0.0f) ||
        ((Particle->RelativeTime - SubUVPayload.RandomImageTime) > LODLevel->RequiredModule->RandomImageTime) ||
        (SubUVPayload.RandomImageTime == 0.0f))
        {
            ImageIndex = static_cast<int>(SubImageIndex.GetValue());
            SubUVPayload.RandomImageTime = Particle->RelativeTime;
        }
    }
    else
    {
        ImageIndex = (ImageIndex + 1) % TotalSubImages;
    }

    return ImageIndex;
}
