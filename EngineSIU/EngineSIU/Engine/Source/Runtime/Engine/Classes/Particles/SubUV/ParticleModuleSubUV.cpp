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
    const int32 PayloadOffset = Owner->SubUVDataOffset;
    if (PayloadOffset == 0)
    {
        return;
    }
    
    SPAWN_INIT
    {
        int32	TempOffset	= CurrentOffset;
        CurrentOffset	= PayloadOffset;
        PARTICLE_ELEMENT(FFullSubUVPayload, SubUVPayload)
        CurrentOffset	= TempOffset;

        SubUVPayload.ImageIndex = DetermineImageIndex(Owner, Offset, &Particle, bRandomSubUV, SubUVPayload, SpawnTime);
    }
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

    int32	TempOffset	= CurrentOffset;
    CurrentOffset	= PayloadOffset;
    PARTICLE_ELEMENT(FFullSubUVPayload, SubUVPayload)
    CurrentOffset	= TempOffset;

    SubUVPayload.ImageIndex = DetermineImageIndex(Owner, Offset, &Particle, bRandomSubUV, SubUVPayload, DeltaTime);
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
    
    const int32 TotalSubImages = LODLevel->RequiredModule->SubImages_Horizontal * LODLevel->RequiredModule->SubImages_Vertical;

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
