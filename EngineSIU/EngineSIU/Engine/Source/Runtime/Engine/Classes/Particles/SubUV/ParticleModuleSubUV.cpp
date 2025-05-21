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

void UParticleModuleSubUV::DisplayProperty()
{
    UParticleModuleSUbUVBase::DisplayProperty();

    for (const auto& Property : StaticClass()->GetProperties())
    {
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
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
    
    BEGIN_UPDATE_LOOP
    if (Particle.RelativeTime > 1.0f)
    {
        CONTINUE_UPDATE_LOOP
    }
    
    uint8* writableBase = const_cast<uint8*>(ParticleBase);
    FFullSubUVPayload* InitialVelPtr = reinterpret_cast<FFullSubUVPayload*>(writableBase + ModulePayloadOffset);

    DetermineImageIndex(Owner, Offset, &Particle, bRandomSubUV, *InitialVelPtr, DeltaTime);

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
    if (TotalSubImages == 0)
    {
        return -1;
    }
    
    int ImageIndex = SubUVPayload.ImageIndex;

    float Time = Particle->RelativeTime / Particle->OneOverMaxLifetime;
    
    if ((LODLevel->RequiredModule->RandomImageTime == 0.0f) ||
    ((Time - SubUVPayload.RandomImageTime) > LODLevel->RequiredModule->RandomImageTime) ||
    (SubUVPayload.RandomImageTime == 0.0f))
    {
        if (bRandomMode)
        {
            ImageIndex = static_cast<int>(SubImageIndex.GetValue());
        }
        else
        {
            ImageIndex = (ImageIndex + 1) % TotalSubImages;
        }
            
        SubUVPayload.RandomImageTime = Particle->RelativeTime / Particle->OneOverMaxLifetime;
    }
    
    SubUVPayload.ImageIndex = ImageIndex;
    return ImageIndex;
}
