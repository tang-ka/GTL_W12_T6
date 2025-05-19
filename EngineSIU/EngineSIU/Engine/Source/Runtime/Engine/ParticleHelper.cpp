#include "ParticleHelper.h"

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    ParticleDataNumBytes = InParticleDataNumBytes;
    ParticleIndicesNumShorts = InParticleIndicesNumShorts;

    MemBlockSize = ParticleDataNumBytes + ParticleIndicesNumShorts * sizeof(uint16);

    //ParticleData = (uint8*)FastParticleSmallBlockAlloc(MemBlockSize);
    //ParticleIndices = (uint16*)(ParticleData + ParticleDataNumBytes);
}

void FParticleDataContainer::Free()
{
    if (ParticleData)
    {
        //FastParticleSmallBlockFree(ParticleData, MemBlockSize);
    }
    MemBlockSize = 0;
    ParticleDataNumBytes = 0;
    ParticleIndicesNumShorts = 0;
    ParticleData = nullptr;
    ParticleIndices = nullptr;
}

FDynamicSpriteEmitterDataBase::FDynamicSpriteEmitterDataBase(const UParticleModuleRequired* RequiredModule)
{
    
}
