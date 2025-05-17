#include "ParticleSystemComponent.h"

UParticleSystemComponent::UParticleSystemComponent()
    : AccumTickTime(0.f)
    , Template(nullptr)
{
}

void UParticleSystemComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}
