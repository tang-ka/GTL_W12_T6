#include "ParticleEmitter.h"

#include "ParticleHelper.h"

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
