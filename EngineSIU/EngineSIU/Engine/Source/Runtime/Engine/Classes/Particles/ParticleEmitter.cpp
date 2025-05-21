#include "ParticleEmitter.h"

#include "ParticleHelper.h"
#include "ParticleLODLevel.h"
#include "ParticleModule.h"
#include "ParticleModuleRequired.h"
#include "Spawn/ParticleModuleSpawn.h"

UParticleEmitter::UParticleEmitter()
{
    UParticleLODLevel* NewParticleLODLevel = new UParticleLODLevel(); // LOD 하나만 쓰기 때문에 일단 이렇게 생성
    LODLevels.Add(NewParticleLODLevel);
}

void UParticleEmitter::CacheEmitterModuleInfo()
{
    // TODO: 언리얼 코드 참고
    
	ParticleSize = sizeof(FBaseParticle);

    ModuleOffsetMap.Empty();

    UParticleLODLevel* CurrentLODLevel = GetLODLevel(0);
    if (CurrentLODLevel == nullptr)
    {
        return;
    }

    ModuleOffsetMap.Add(CurrentLODLevel->RequiredModule, CurrentLODLevel->RequiredModule->ModulePayloadOffset);
    ModuleOffsetMap.Add(CurrentLODLevel->SpawnModule, CurrentLODLevel->SpawnModule->ModulePayloadOffset);

    for (const auto& Module : CurrentLODLevel->GetModules())
    {
        ModuleOffsetMap.Add(Module, Module->ModulePayloadOffset);
    }
}

void UParticleEmitter::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}

UParticleLODLevel* UParticleEmitter::GetLODLevel(int32 LODIndex) const
{
    if (LODLevels.IsValidIndex(LODIndex))
    {
        return LODLevels[LODIndex];
    }
    return nullptr;
}
