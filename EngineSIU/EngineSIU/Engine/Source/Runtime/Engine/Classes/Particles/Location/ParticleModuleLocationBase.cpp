#include "ParticleModuleLocationBase.h"

void UParticleModuleLocationBase::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}
