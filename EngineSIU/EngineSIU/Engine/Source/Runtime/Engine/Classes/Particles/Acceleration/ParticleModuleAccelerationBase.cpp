#include "ParticleModuleAccelerationBase.h"

void UParticleModuleAccelerationBase::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}
