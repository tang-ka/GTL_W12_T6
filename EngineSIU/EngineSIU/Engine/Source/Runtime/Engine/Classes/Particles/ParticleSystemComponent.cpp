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

void UParticleSystemComponent::UpdateDynamicData()
{
    // Create the dynamic data for rendering this particle system
    FParticleDynamicData* ParticleDynamicData = CreateDynamicData();
}

FParticleDynamicData* UParticleSystemComponent::CreateDynamicData()
{
    if (EmitterInstances.Num() > 0)
    {
        int32 LiveCount = 0;
        for (int32 EmitterIndex = 0; EmitterIndex < EmitterInstances.Num(); EmitterIndex++)
        {
            FParticleEmitterInstance* EmitInst = EmitterInstances[EmitterIndex];
            if (EmitInst)
            {
                if (EmitInst->ActiveParticles > 0)
                {
                    LiveCount++;
                }
            }
        }

        if (LiveCount == 0)
        {
            return nullptr;
        }
    }

    FParticleDynamicData* ParticleDynamicData = new FParticleDynamicData();

    if (Template)
    {
        ParticleDynamicData->SystemPositionForMacroUVs = GetComponentTransform().TransformPosition(Template->GetMacroUVPosition());
        ParticleDynamicData->SystemRadiusForMacroUVs = Template->GetMacroUVRadius();
    }

    return ParticleDynamicData;
}
