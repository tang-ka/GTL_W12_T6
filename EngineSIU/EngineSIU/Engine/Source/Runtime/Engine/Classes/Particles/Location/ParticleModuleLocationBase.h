#pragma once
#include "Particles/ParticleModule.h"

class UParticleModuleLocationBase : public UParticleModule
{
    DECLARE_CLASS(UParticleModuleLocationBase, UParticleModule)
public:
    UParticleModuleLocationBase() = default;

    virtual void DisplayProperty() override;

    // 월드 좌표로 해석할지, 로컬 좌표(Emitter Origin 기준)로 해석할지
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bInWorldSpace)
    // Emitter 위치를 시작 위치에 더할지 여부
    UPROPERTY_WITH_FLAGS(EditAnywhere, bool, bApplyEmitterLocation)
};

