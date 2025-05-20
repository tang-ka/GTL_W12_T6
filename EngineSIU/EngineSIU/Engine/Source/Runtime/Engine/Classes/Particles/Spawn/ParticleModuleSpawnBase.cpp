#include "ParticleModuleSpawnBase.h"

void UParticleModuleSpawnBase::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}
