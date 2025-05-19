#include "ParticleHelper.h"

void FParticleDataContainer::Alloc(int32 InParticleDataNumBytes, int32 InParticleIndicesNumShorts)
{
    ParticleDataNumBytes = InParticleDataNumBytes;
    ParticleIndicesNumShorts = InParticleIndicesNumShorts;

    MemBlockSize = ParticleDataNumBytes + ParticleIndicesNumShorts * sizeof(uint16);

    // ParticleData = (uint8*)FastParticleSmallBlockAlloc(MemBlockSize); // TODO: 주석 풀기
    ParticleIndices = (uint16*)(ParticleData + ParticleDataNumBytes);
}

void FParticleDataContainer::Free()
{
    // if (ParticleData)
    // {
        // FastParticleSmallBlockFree(ParticleData, MemBlockSize); // TODO: 주석 풀기
    // }
    MemBlockSize = 0;
    ParticleDataNumBytes = 0;
    ParticleIndicesNumShorts = 0;
    ParticleData = nullptr;
    ParticleIndices = nullptr;
}
