#include "ParticleEmitter.h"

#include "ParticleHelper.h"
#include "ParticleLODLevel.h"
#include "ParticleModule.h"
#include "Spawn/ParticleModuleSpawn.h"

UParticleEmitter::UParticleEmitter()
{
    //////////////// 테스트 코드
    UParticleLODLevel* NewParticleLODLevel = new UParticleLODLevel(); // LOD 하나만 쓰기 때문에 일단 이렇게 생성
    LODLevels.Add(NewParticleLODLevel);

    UParticleModule* RequiredModule = new UParticleModule();
    RequiredModule->ModuleName = "RequiredModule";
    NewParticleLODLevel->Modules.Add(RequiredModule);

    UParticleModuleSpawn* SpawnModule = new UParticleModuleSpawn();
    SpawnModule->ModuleName = "SpawnModule";
    NewParticleLODLevel->Modules.Add(SpawnModule);

    UParticleModule* InitialScaleModule = new UParticleModule();
    InitialScaleModule->ModuleName = "InitialScaleModule";
    NewParticleLODLevel->Modules.Add(InitialScaleModule);
    ///////////////////////////
}

void UParticleEmitter::CacheEmitterModuleInfo()
{
    // TODO: 언리얼 코드 참고
    
	ParticleSize = sizeof(FBaseParticle);
	
}
