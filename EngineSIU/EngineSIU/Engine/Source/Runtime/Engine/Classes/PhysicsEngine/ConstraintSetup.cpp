#include "ConstraintSetup.h"

UConstraintSetup::UConstraintSetup()
    : TwistLimitAngle(0.0f)
    , SwingLimitAngle(0.0f)
    , LinearLimitSize(0.0f)
{
}

void UConstraintSetup::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}

