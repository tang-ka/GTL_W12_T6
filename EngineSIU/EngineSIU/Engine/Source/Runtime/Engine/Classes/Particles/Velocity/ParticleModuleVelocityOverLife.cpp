#include "ParticleModuleVelocityOverLife.h"
#include "Engine/ParticleHelper.h"
#include "Engine/ParticleEmitterInstance.h"
#include "Particles/ParticleSystemComponent.h"

UParticleModuleVelocityOverLife::UParticleModuleVelocityOverLife()
{
    bSpawnModule = true;
    bSpawnModule = true;

    bInWorldSpace = false;
    bApplyOwnerScale = false;

    bUseConstantChange = true;
    bUseVelocityCurve = false;

    CurveScale = 1.0f;

    StartVelocity = FVector::ZeroVector;
    EndVelocity = FVector::ZeroVector;
}

void UParticleModuleVelocityOverLife::DisplayProperty()
{
    Super::DisplayProperty();

    // 상호 배타적 옵션 동기화
    if (bUseConstantChange && bUseVelocityCurve)
    {
        if (ImGui::IsItemEdited()) bUseVelocityCurve = false;
    }
    else if (bUseVelocityCurve && bUseConstantChange)
    {
        if (ImGui::IsItemEdited()) bUseConstantChange = false;
    }

    for (const auto& Property : StaticClass()->GetProperties())
    {
        ImGui::PushID(Property);
        Property->DisplayInImGui(this);
        ImGui::PopID();
    }
}

void UParticleModuleVelocityOverLife::Spawn(FParticleEmitterInstance* Owner, int32 Offset, float SpawnTime, FBaseParticle* ParticleBase)
{
}

void UParticleModuleVelocityOverLife::Update(FParticleEmitterInstance* Owner, int32 Offset, float DeltaTime)
{
    BEGIN_UPDATE_LOOP
                          
    float RelTime = Particle.RelativeTime;
    FVector NewVelocity = FVector::ZeroVector;
    
    if (bUseConstantChange)
    {
        float Alpha = FMath::Clamp(RelTime, 0.0f, 1.0f);    
        NewVelocity = FMath::Lerp(StartVelocity, EndVelocity, Alpha);
    }
    else if (bUseVelocityCurve)
    {
        FVector CurrentVelocity = Particle.BaseVelocity;
        float Alpha = CurveScale * DeltaTime;
        NewVelocity = FMath::Lerp(CurrentVelocity, EndVelocity, Alpha);
    }

    // 월드공간 변환 및 오너 스케일 적용
    if (bInWorldSpace)
    {
        NewVelocity = Owner->Component->GetComponentToWorld().TransformVector(NewVelocity);
    }
    if (bApplyOwnerScale)
    {
        NewVelocity *= Owner->Component->GetComponentToWorld().GetScale3D();
    }

    Particle.BaseVelocity = NewVelocity;
            
    END_UPDATE_LOOP
}
