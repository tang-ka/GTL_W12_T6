#pragma once
#include "ParticleModule.h"

enum EParticleSortMode : int
{
    PSORTMODE_None,
    PSORTMODE_ViewProjDepth,
    PSORTMODE_DistanceToView,
    PSORTMODE_Age_OldestFirst,
    PSORTMODE_Age_NewestFirst,
    PSORTMODE_MAX,
};

struct FParticleRequiredModule
{
    uint32 NumFrames;
    uint32 NumBoundingVertices;
    uint32 NumBoundingTriangles;
    float AlphaThreshold;
    TArray<FVector2D> FrameData;
    ID3D11ShaderResourceView* BoundingGeometryBufferSRV;
    uint8 bCutoutTextureIsValid : 1;
    uint8 bUseVelocityForMotionBlur : 1;
};

struct FParticleRequiredModule;
class UParticleModuleRequired : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleRequired, UParticleModule)
public:
    UParticleModuleRequired() = default;

    virtual void DisplayProperty() override;

public:
    FParticleRequiredModule* CreateRendererResource();

    UPROPERTY_WITH_FLAGS(EditAnywhere, UMaterial*, MaterialInterface)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseMaxDrawCount)
    UPROPERTY_WITH_FLAGS(EditAnywhere, uint32, MaxDrawCount)
};
