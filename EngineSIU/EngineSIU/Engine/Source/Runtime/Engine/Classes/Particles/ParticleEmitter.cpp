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
    ///////////////////////////
}

void UParticleEmitter::CacheEmitterModuleInfo()
{
    // TODO: 언리얼 코드 참고
    
	ParticleSize = sizeof(FBaseParticle);
	
}

UParticleLODLevel* UParticleEmitter::GetLODLevel(int32 LODIndex) const
{
    if (LODLevels.IsValidIndex(LODIndex))
    {
        return LODLevels[LODIndex];
    }
    return nullptr;
}
