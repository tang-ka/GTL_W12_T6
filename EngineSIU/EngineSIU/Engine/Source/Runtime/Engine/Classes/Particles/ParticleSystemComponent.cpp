#include "ParticleSystemComponent.h"
#include "ParticleEmitterInstance.h"
#include "Particles/ParticleSystem.h"

UParticleSystemComponent::UParticleSystemComponent()
    : AccumTickTime(0.f)
    , Template(nullptr)
{
}

UObject* UParticleSystemComponent::Duplicate(UObject* InOuter)
{
    return nullptr;
}

void UParticleSystemComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (Template)
    {
        InitializeSystem();
    }
}

void UParticleSystemComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    for (auto* Instance : EmitterInstances)
    {
        if (Instance)
        {
            Instance->Tick(DeltaTime);
        }
    }
}

void UParticleSystemComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
}

void UParticleSystemComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
}

void UParticleSystemComponent::InitializeSystem()
{
    TArray<UParticleEmitter*> Emitters = Template->GetEmitters();
    for (int32 i = 0; i < Emitters.Num(); i++)
    {
        UParticleEmitter* EmitterTemplate = Emitters[i];
        CreateAndAddEmitterInstance(EmitterTemplate);
    }
}

void UParticleSystemComponent::CreateAndAddEmitterInstance(UParticleEmitter* EmitterTemplate)
{
    if (EmitterTemplate)
    {
        FParticleEmitterInstance* Instance = new FParticleEmitterInstance();
        Instance->SpriteTemplate = EmitterTemplate;
        Instance->Component = this;
        Instance->CurrentLODLevelIndex = 0;

        Instance->Initialize();

        EmitterInstances.Add(Instance);
    }
}

