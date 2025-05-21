#pragma once

#include "ParticleModule.h"
#include "Components/Material/Material.h"

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
    UParticleModuleRequired();

    virtual void DisplayProperty() override;

public:
    FParticleRequiredModule* CreateRendererResource();

    void GenerateSubUVFrameData(FParticleRequiredModule* OutData) const;
    void SetupCutoutGeometryData(FParticleRequiredModule* OutData) const;
    void SetupMotionBlurFlag(FParticleRequiredModule* OutData) const;

public:
    UPROPERTY_WITH_FLAGS(EditAnywhere, UMaterial*, MaterialInterface, = nullptr)
    EModuleType ModuleType = EModuleType::EPMT_Required;
    EParticleSortMode SortMode = EParticleSortMode::PSORTMODE_None;

    // SubUV 관련
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, RandomImageTime, = 0.1f)
    UPROPERTY_WITH_FLAGS(EditAnywhere, int32, SubImagesHorizontal, =0)
    UPROPERTY_WITH_FLAGS(EditAnywhere, int32, SubImagesVertical, =0)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bSubUVRandomMode, =false)

    // 드로우 제한 관련
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseMaxDrawCount)
    UPROPERTY_WITH_FLAGS(EditAnywhere, uint32, MaxDrawCount)

    // 지속 시간 관련
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, EmitterDuration)
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, EmitterDurationLow)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bEmitterDurationUseRange)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bDurationRecalcEachLoop)

    // 지연 시간 관련
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, Delay)
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, DelayLow)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bDelayFirstOnly)

    // 스폰 속도 관련
    //UPROPERTY_WITH_FLAGS(EditAnywhere, float, SpawnRate)
    //UPROPERTY_WITH_FLAGS(EditAnywhere, float, SpawnRateScale)

    // 좌표계/속도 관련
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseLocalSpace)
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, EmitterVelocityScale)
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, InheritVelocityScale)

    // 위치 관련
    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector2D, EmitterOrigin)

    // 종료 조건
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bKillOnDeactivate)
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bKillOnCompleted)


    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseCutoutMask)
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, AlphaThreshold)

    TArray<FVector2D> CachedBoundingGeometry;
    ID3D11ShaderResourceView* CachedBoundingGeometrySRV = nullptr;

    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bUseVelocityForMotionBlur)
};
