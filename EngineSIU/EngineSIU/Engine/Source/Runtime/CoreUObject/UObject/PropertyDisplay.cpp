#include "Property.h"
#include "Math/NumericLimits.h"
#include "ImGui/imgui.h"

struct FPropertyUIHelper
{
    template <typename T>
    static void DisplayNumericDrag(const FProperty& Prop, UObject* Object, float Speed = 1.0f)
    {
        T* Data = Prop.GetPropertyData<T>(Object);
        constexpr T Min = TNumericLimits<T>::Min();
        constexpr T Max = TNumericLimits<T>::Max();
    
        ImGuiDataType DataType;
        if constexpr (std::same_as<T, int8>)        { DataType = ImGuiDataType_S8;     }
        else if constexpr (std::same_as<T, int16>)  { DataType = ImGuiDataType_S16;    }
        else if constexpr (std::same_as<T, int32>)  { DataType = ImGuiDataType_S32;    }
        else if constexpr (std::same_as<T, int64>)  { DataType = ImGuiDataType_S64;    }
        else if constexpr (std::same_as<T, uint8>)  { DataType = ImGuiDataType_U8;     }
        else if constexpr (std::same_as<T, uint16>) { DataType = ImGuiDataType_U16;    }
        else if constexpr (std::same_as<T, uint32>) { DataType = ImGuiDataType_U32;    }
        else if constexpr (std::same_as<T, uint64>) { DataType = ImGuiDataType_U64;    }
        else if constexpr (std::same_as<T, float>)  { DataType = ImGuiDataType_Float;  }
        else if constexpr (std::same_as<T, double>) { DataType = ImGuiDataType_Double; }
        else { static_assert(false); }
    
        ImGui::DragScalar(Prop.Name, DataType, Data, Speed, &Min, &Max);
    }
};


void FProperty::DisplayInImGui(UObject* Object) const
{
}

void FInt8Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<int8>(*this, Object);
}

void FInt16Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<int16>(*this, Object);
}

void FInt32Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<int32>(*this, Object);
}

void FInt64Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<int64>(*this, Object);
}

void FUInt8Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<uint8>(*this, Object);
}

void FUInt16Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<uint16>(*this, Object);
}

void FUInt32Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<uint32>(*this, Object);
}

void FUInt64Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<uint64>(*this, Object);
}

void FFloatProperty::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<float>(*this, Object);
}

void FDoubleProperty::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDrag<double>(*this, Object);
}

void FBoolProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    bool* Data = GetPropertyData<bool>(Object);
    ImGui::Checkbox(Name, Data);
}

void FStrProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);
    FString* Data = GetPropertyData<FString>(Object);

    constexpr int32 IMGUI_FSTRING_BUFFER_SIZE = 2048;
    char Buffer[IMGUI_FSTRING_BUFFER_SIZE];
    FCStringAnsi::Strncpy(Buffer, Data->ToAnsiString().c_str(), IMGUI_FSTRING_BUFFER_SIZE);
    Buffer[IMGUI_FSTRING_BUFFER_SIZE - 1] = '\0'; // 항상 널 종료 보장

    if (ImGui::InputText(Name, Buffer, IMGUI_FSTRING_BUFFER_SIZE))
    {
        *Data = Buffer;
    }
}
