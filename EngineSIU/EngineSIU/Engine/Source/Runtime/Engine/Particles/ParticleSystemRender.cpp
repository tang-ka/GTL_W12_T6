#include "ParticleHelper.h"
#include "Particles/ParticleSpriteEmitter.h"
#include "Particles/ParticleModuleRequired.h"
#include "Math/Matrix.h"

void FDynamicSpriteEmitterDataBase::SortSpriteParticles(int32 SortMode, bool bLocalSpace, int32 ParticleCount, const uint8* ParticleData, int32 ParticleStride, const uint16* ParticleIndices, const FMatrix& LocalToWorld) const
{
    if (SortMode == PSORTMODE_ViewProjDepth)
	{

	}
	else if (SortMode == PSORTMODE_DistanceToView)
	{

	}
	else if (SortMode == PSORTMODE_Age_OldestFirst)
	{

	}
	else if (SortMode == PSORTMODE_Age_NewestFirst)
	{

	}
}

FVector2D GetParticleSize(const FBaseParticle& Particle, const FDynamicSpriteEmitterReplayDataBase& Source)
{
    FVector2D Size;
    Size.X = FMath::Abs(Particle.Size.X * Source.Scale.X);
    Size.Y = FMath::Abs(Particle.Size.Y * Source.Scale.Y);
    if (Source.ScreenAlignment == PSA_Square || Source.ScreenAlignment == PSA_FacingCameraPosition || Source.ScreenAlignment == PSA_FacingCameraDistanceBlend)
    {
        Size.Y = Size.X;
    }

    return Size;
}

void FDynamicSpriteEmitterData::Init(bool bInSelected)
{
    bSelected = bInSelected;

    bUsesDynamicParameter = GetSourceData()->DynamicParameterDataOffset > 0;

    // We won't need this on the render thread
    Source.MaterialInterface = nullptr;
}

bool FDynamicSpriteEmitterData::GetVertexAndIndexData(void* VertexData, void* FillIndexData, const FVector& InCameraPosition, const FMatrix& InLocalToWorld, uint32 InstanceFactor) const
{
    int32 ParticleCount = Source.ActiveParticleCount;
    if ((Source.MaxDrawCount >= 0) && (ParticleCount > Source.MaxDrawCount))
    {
        ParticleCount = Source.MaxDrawCount;
    }

    // Put the camera origin in the appropriate coordinate space.
    FVector CameraPosition = InCameraPosition;
    if (Source.bUseLocalSpace)
    {
        FMatrix InvSelf = FMatrix::Inverse(InLocalToWorld);
        CameraPosition = InvSelf.TransformPosition(InCameraPosition);
    }

    // Pack the data
    int32 ParticleIndex = 0;

    int32 VertexStride = sizeof(FParticleSpriteVertex);
    uint8* TempVert = (uint8*)VertexData;

    FParticleSpriteVertex* FillVertex;

    float SubImageIndex = 0.0f;

    const uint8* ParticleData = Source.DataContainer.ParticleData;
    const uint16* ParticleIndices = Source.DataContainer.ParticleIndices;
    const int32 OrderedIndices = 0; // Todo: Need to change the real index.

    for (int32 i = 0; i < ParticleCount; i++)
    {
        ParticleIndex = OrderedIndices ? 0 : i; // Todo: Need to change the real index.
        DECLARE_PARTICLE_CONST(Particle, ParticleData + Source.ParticleStride * ParticleIndices[ParticleIndex])

        // Prefetch
        if (i + 1 < ParticleCount)
        {
            int32 NextIndex = OrderedIndices ? /*OrderedIndices[*/ i + 1 /*]*/ : (i + 1);
            DECLARE_PARTICLE_CONST(NextParticle, ParticleData + Source.ParticleStride * ParticleIndices[NextIndex])
            // Prefetch(&NextParticle);
        }

        const FVector2D Size = GetParticleSize(Particle, Source);

        FVector ParticlePosition = Particle.Location;
        FVector ParticleOldPosition = Particle.OldLocation;
        
        if (Source.CameraPayloadOffset != 0)
        {
            
        }

        if (Source.SubUVDataOffset > 0)
        {
            
        }

        for (uint32 Factor = 0; Factor < InstanceFactor; Factor++)
        {
            FillVertex = reinterpret_cast<FParticleSpriteVertex*>(TempVert);
            FillVertex->Position = FVector(ParticlePosition);
            FillVertex->RelativeTime = Particle.RelativeTime;
            FillVertex->OldPosition = FVector(ParticleOldPosition);
            
            // Create a floating point particle ID from the counter, map into approximately 0-1
            FillVertex->ParticleId = (Particle.Flags & (~0xFE000000)) / 10000.0f;
            FillVertex->Size = FVector2D(GetParticleSizeWithUVFlipInSign(Particle, Size));
            FillVertex->Rotation = Particle.Rotation;
            FillVertex->SubImageIndex = SubImageIndex;
            FillVertex->Color = Particle.Color;

            TempVert += VertexStride;
        }
    }
    
    return true;
}

void FDynamicSpriteEmitterData::GetDynamicMeshElementsEmitter() const
{
    
}
