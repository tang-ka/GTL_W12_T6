#include "ParticleModuleRequired.h"

#include "Engine/AssetManager.h"

UParticleModuleRequired::UParticleModuleRequired()
{
    // 기본 재질 없음 (필수로 설정되어야 함)
    TSet<FName> MaterialKeys;
    UAssetManager::Get().GetMaterialKeys(MaterialKeys);
    MaterialInterface = UAssetManager::Get().GetMaterial(*MaterialKeys.begin());

    // 모듈 타입은 Required로 고정
    ModuleType = EModuleType::EPMT_Required;

    // 정렬은 기본적으로 없음 (뷰에 따라 깊이 정렬하는 것도 있음)
    SortMode = EParticleSortMode::PSORTMODE_None;

    // 드로우 제한은 끄는 게 기본. 끄면 무제한 렌더링됨.
    bUseMaxDrawCount = false;
    MaxDrawCount = 1000;

    // Emitter Duration: 10초 동안 반복 (값이 0이면 무한 반복처럼 처리할 수도 있음)
    EmitterDuration = 10.0f;
    EmitterDurationLow = 1.0f;
    bEmitterDurationUseRange = false;
    bDurationRecalcEachLoop = false;

    // Delay는 기본적으로 없음
    Delay = 0.0f;
    DelayLow = 0.0f;
    bDelayFirstOnly = true;

    // 초당 스폰 10개, 스케일 1.0
    //SpawnRate = 10.0f;
    //SpawnRateScale = 1.0f;

    // 로컬 좌표계 사용 여부 (false면 월드 스페이스 기준)
    bUseLocalSpace = false;
    EmitterVelocityScale = 1.0f;
    InheritVelocityScale = 0.0f;

    // 기본 오리진은 (0,0)
    EmitterOrigin = FVector2D::ZeroVector;

    // 종료 조건: 비활성화되거나 완료되면 파티클 정리
    bKillOnDeactivate = true;
    bKillOnCompleted = true;

    SubImagesHorizontal = 1;
    SubImagesVertical = 1;

    bUseCutoutMask = false;
    AlphaThreshold = 0.5f;
    CachedBoundingGeometry.Empty();
    CachedBoundingGeometrySRV = nullptr;

    bUseVelocityForMotionBlur = false;

    ModuleName = "Required";
}

void UParticleModuleRequired::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}

FParticleRequiredModule* UParticleModuleRequired::CreateRendererResource()
{
    FParticleRequiredModule* FReqMod = new FParticleRequiredModule();

    GenerateSubUVFrameData(FReqMod);

    SetupCutoutGeometryData(FReqMod);

    SetupMotionBlurFlag(FReqMod);

    return FReqMod;
}

void UParticleModuleRequired::GenerateSubUVFrameData(FParticleRequiredModule* OutData) const
{
    if (SubImagesHorizontal <= 0 || SubImagesVertical <= 0)
    {
        OutData->NumFrames = 1; // SubUV 안 쓴다면 1
        OutData->FrameData = { FVector2D::ZeroVector }; // UV 좌표 기본값
        return;
    }

    OutData->NumFrames = SubImagesHorizontal * SubImagesVertical;
    OutData->FrameData.Empty();
    OutData->FrameData.Reserve(OutData->NumFrames);

    for (int32 y = 0; y < SubImagesVertical; ++y)
    {
        for (int32 x = 0; x < SubImagesHorizontal; ++x)
        {
            float U = static_cast<float>(x) / static_cast<float>(SubImagesHorizontal);
            float V = static_cast<float>(y) / static_cast<float>(SubImagesVertical);
            OutData->FrameData.Add(FVector2D(U, V));
        }
    }
}

void UParticleModuleRequired::SetupCutoutGeometryData(FParticleRequiredModule* OutData) const
{
    OutData->bCutoutTextureIsValid = bUseCutoutMask; // 임시

    OutData->NumBoundingVertices = CachedBoundingGeometry.Num();
    OutData->NumBoundingTriangles = CachedBoundingGeometry.Num() / 3;
    OutData->BoundingGeometryBufferSRV = CachedBoundingGeometrySRV;

    OutData->AlphaThreshold = AlphaThreshold; // 임계값 기본 설정
}

void UParticleModuleRequired::SetupMotionBlurFlag(FParticleRequiredModule* OutData) const
{
    OutData->bUseVelocityForMotionBlur = bUseVelocityForMotionBlur;
}
