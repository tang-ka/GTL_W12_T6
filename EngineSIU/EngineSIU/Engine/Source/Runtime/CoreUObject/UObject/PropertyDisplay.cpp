#include "Property.h"

#include "Class.h"
#include "UObjectHash.h"
#include "Editor/UnrealEd/ImGuiWidget.h"
#include "Math/NumericLimits.h"
#include "Template/SubclassOf.h"

#include "ImGui/imgui.h"

template <typename Type, typename... Types>
concept TIsAnyOf = (std::same_as<Type, Types> || ...);

template <typename T>
concept NumericType = TIsAnyOf<T, int8, int16, int32, int64, uint8, uint16, uint32, uint64, float, double>;

static constexpr int32 IMGUI_FSTRING_BUFFER_SIZE = 2048;


struct FPropertyUIHelper
{
    template <NumericType NumType>
    static void DisplayNumericDragN(const char* PropertyLabel, void* InData, int Components, float Speed = 1.0f, const char* Format = nullptr)
    {
        NumType* Data = static_cast<NumType*>(InData);
        constexpr NumType Min = TNumericLimits<NumType>::Lowest();
        constexpr NumType Max = TNumericLimits<NumType>::Max();

        ImGuiDataType DataType;
        if constexpr (std::same_as<NumType, int8>)        { DataType = ImGuiDataType_S8;     }
        else if constexpr (std::same_as<NumType, int16>)  { DataType = ImGuiDataType_S16;    }
        else if constexpr (std::same_as<NumType, int32>)  { DataType = ImGuiDataType_S32;    }
        else if constexpr (std::same_as<NumType, int64>)  { DataType = ImGuiDataType_S64;    }
        else if constexpr (std::same_as<NumType, uint8>)  { DataType = ImGuiDataType_U8;     }
        else if constexpr (std::same_as<NumType, uint16>) { DataType = ImGuiDataType_U16;    }
        else if constexpr (std::same_as<NumType, uint32>) { DataType = ImGuiDataType_U32;    }
        else if constexpr (std::same_as<NumType, uint64>) { DataType = ImGuiDataType_U64;    }
        else if constexpr (std::same_as<NumType, float>)  { DataType = ImGuiDataType_Float;  }
        else if constexpr (std::same_as<NumType, double>) { DataType = ImGuiDataType_Double; }
        else { static_assert(TAlwaysFalse<NumType>); }

        ImGui::Text("%s", PropertyLabel);
        ImGui::SameLine();
        ImGui::DragScalarN(std::format("##{}", PropertyLabel).c_str(), DataType, Data, Components, Speed, &Min, &Max, Format);
    }
};


void FProperty::DisplayInImGui(UObject* Object) const
{
    void* Data = GetPropertyData(Object);
    DisplayRawDataInImGui(Name, Data);
}

void FProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
}

void FInt8Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int8>(PropertyLabel, DataPtr, 1);
}

void FInt16Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int16>(PropertyLabel, DataPtr, 1);
}

void FInt32Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int32>(PropertyLabel, DataPtr, 1);
}

void FInt64Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<int64>(PropertyLabel, DataPtr, 1);
}

void FUInt8Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint8>(PropertyLabel, DataPtr, 1);
}

void FUInt16Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint16>(PropertyLabel, DataPtr, 1);
}

void FUInt32Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint32>(PropertyLabel, DataPtr, 1);
}

void FUInt64Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<uint64>(PropertyLabel, DataPtr, 1);
}

void FFloatProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 1);
}

void FDoubleProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FNumericProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<double>(PropertyLabel, DataPtr, 1);
}

void FBoolProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    ImGui::Checkbox(PropertyLabel, static_cast<bool*>(DataPtr));
}

void FStrProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FString* Data = static_cast<FString*>(DataPtr);

    char Buffer[IMGUI_FSTRING_BUFFER_SIZE];
    FCStringAnsi::Strncpy(Buffer, Data->ToAnsiString().c_str(), IMGUI_FSTRING_BUFFER_SIZE);
    Buffer[IMGUI_FSTRING_BUFFER_SIZE - 1] = '\0'; // 항상 널 종료 보장

    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    if (ImGui::InputText(std::format("##{}", PropertyLabel).c_str(), Buffer, IMGUI_FSTRING_BUFFER_SIZE))
    {
        *Data = Buffer;
    }
}

void FNameProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    const FName* Data = static_cast<FName*>(DataPtr);
    std::string NameStr = Data->ToString().ToAnsiString();

    // ReadOnly Mode
    ImGui::BeginDisabled(true);
    {
        ImGui::Text("%s", PropertyLabel);
        ImGui::SameLine();
        ImGui::InputText(std::format("##{}", PropertyLabel).c_str(), NameStr.data(), NameStr.size(), ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::EndDisabled();
}

void FVector2DProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 2);
}

void FVectorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 3);
}

void FVector4Property::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 4);
}

void FRotatorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 3);
}

void FQuatProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FPropertyUIHelper::DisplayNumericDragN<float>(PropertyLabel, DataPtr, 4);
}

void FTransformProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    if (ImGui::TreeNode(PropertyLabel))
    {
        FTransform* Data = static_cast<FTransform*>(DataPtr);
        FRotator Rotation = Data->Rotator();

        FImGuiWidget::DrawVec3Control("Location", Data->Translation);
        FImGuiWidget::DrawRot3Control("Rotation", Rotation);
        FImGuiWidget::DrawVec3Control("Scale", Data->Scale3D, 1.0f);

        Data->Rotation = Rotation.Quaternion();
        ImGui::TreePop();
    }
}

void FMatrixProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    // TODO: 짐벌락 현상 있음
    if (ImGui::TreeNode(PropertyLabel))
    {
        bool bChanged = false;
        FMatrix* Data = static_cast<FMatrix*>(DataPtr);

        FTransform Transform = FTransform(*Data);
        FRotator Rotation = Transform.Rotator();

        bChanged |= FImGuiWidget::DrawVec3Control("Location", Transform.Translation);
        bChanged |= FImGuiWidget::DrawRot3Control("Rotation", Rotation);
        bChanged |= FImGuiWidget::DrawVec3Control("Scale", Transform.Scale3D, 1.0f);

        if (bChanged)
        {
            *Data =
                FMatrix::CreateScaleMatrix(Transform.Scale3D)
                * FMatrix::CreateRotationMatrix(Rotation.Quaternion())
                * FMatrix::CreateTranslationMatrix(Transform.Translation);
        }

        if (ImGui::TreeNode("Advanced"))
        {
            ImGui::DragFloat4("##1", Data->M[0], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
            ImGui::DragFloat4("##2", Data->M[1], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
            ImGui::DragFloat4("##3", Data->M[2], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
            ImGui::DragFloat4("##4", Data->M[3], 0.01f, -FLT_MAX, FLT_MAX, "%.3f");
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void FColorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FColor* Data = static_cast<FColor*>(DataPtr);
    FLinearColor LinearColor = FLinearColor(*Data);

    constexpr ImGuiColorEditFlags Flags =
        ImGuiColorEditFlags_DisplayRGB
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    if (ImGui::ColorEdit4(std::format("##{}", PropertyLabel).c_str(), reinterpret_cast<float*>(&LinearColor), Flags))
    {
        *Data = LinearColor.ToColorRawRGB8();
    }
}

void FLinearColorProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    FLinearColor* Data = static_cast<FLinearColor*>(DataPtr);

    constexpr ImGuiColorEditFlags Flags =
        ImGuiColorEditFlags_Float
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    ImGui::ColorEdit4(std::format("##{}", PropertyLabel).c_str(), reinterpret_cast<float*>(Data), Flags);
}

void FSubclassOfProperty::DisplayRawDataInImGui(const char* PropertyLabel, void* DataPtr) const
{
    FProperty::DisplayRawDataInImGui(PropertyLabel, DataPtr);

    TSubclassOf<UObject>* Data = static_cast<TSubclassOf<UObject>*>(DataPtr);
    UClass* CurrentClass = GetSpecificClass();
    if (CurrentClass == nullptr)
    {
        return;
    }

    TArray<UClass*> ChildClasses;
    GetChildOfClass(CurrentClass, ChildClasses);

    const std::string CurrentClassName = (*Data) ? (*Data)->GetName().ToAnsiString() : "None";
    ImGui::Text("%s", PropertyLabel);
    ImGui::SameLine();
    if (ImGui::BeginCombo(std::format("##{}", PropertyLabel).c_str(), CurrentClassName.c_str()))
    {
        if (ImGui::Selectable("None", !(*Data)))
        {
            *Data = nullptr;
        }

        for (UClass* ChildClass : ChildClasses)
        {
            const std::string ChildClassName = ChildClass->GetName().ToAnsiString();
            const bool bIsSelected = (*Data) && (*Data) == ChildClass;
            if (ImGui::Selectable(ChildClassName.c_str(), bIsSelected))
            {
                *Data = ChildClass;
            }
            if (bIsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

void FObjectBaseProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    // 띄울 정보가 딱히 없는듯
}

void FUnresolvedPtrProperty::DisplayInImGui(UObject* Object) const
{
    if (Type == EPropertyType::Object)
    {
        FObjectBaseProperty::DisplayInImGui(Object);
    }
}
