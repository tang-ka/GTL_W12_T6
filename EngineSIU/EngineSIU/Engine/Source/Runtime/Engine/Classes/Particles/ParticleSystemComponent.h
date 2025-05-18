#pragma once
#include "Components/PrimitiveComponent.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleSystem;

class UFXSystemComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UFXSystemComponent, UPrimitiveComponent)

public:
    UFXSystemComponent() = default;
    virtual ~UFXSystemComponent() override = default;
};


class UParticleSystemComponent : public UFXSystemComponent
{
    DECLARE_CLASS(UParticleSystemComponent, UFXSystemComponent)

public:
    UParticleSystemComponent();
    virtual ~UParticleSystemComponent() override = default;

    virtual void TickComponent(float DeltaTime) override;

    UParticleSystem* GetParticleSystem() const { return Template; }
    void SetParticleSystem(UParticleSystem* InParticleSystem) { Template = InParticleSystem; }

    float AccumTickTime;
    
private:
    UParticleSystem* Template = nullptr;
};
