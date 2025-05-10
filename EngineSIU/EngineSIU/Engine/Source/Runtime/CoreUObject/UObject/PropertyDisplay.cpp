#include "Property.h"
#include "ImGui/imgui.h"


void FProperty::DisplayInImGui(UObject* Object) const
{
    // TODO: FProperty를 상속받은 클래스 만들기
}

void FInt8Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FInt16Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FInt32Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FInt64Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FUInt8Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FUInt16Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FUInt32Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FUInt64Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FFloatProperty::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FDoubleProperty::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);
}

void FBoolProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);
}
