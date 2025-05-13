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
    static void DisplayNumericDragN(const FProperty& Prop, UObject* Object, int Components, float Speed = 1.0f, const char* Format = nullptr)
    {
        NumType* Data = Prop.GetPropertyData<NumType>(Object);
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
        else { static_assert(false); }
    
        ImGui::DragScalarN(Prop.Name, DataType, Data, Components, Speed, &Min, &Max, Format);
    }
};


void FProperty::DisplayInImGui(UObject* Object) const
{
}

void FInt8Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<int8>(*this, Object, 1);
}

void FInt16Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<int16>(*this, Object, 1);
}

void FInt32Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<int32>(*this, Object, 1);
}

void FInt64Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<int64>(*this, Object, 1);
}

void FUInt8Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<uint8>(*this, Object, 1);
}

void FUInt16Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<uint16>(*this, Object, 1);
}

void FUInt32Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<uint32>(*this, Object, 1);
}

void FUInt64Property::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<uint64>(*this, Object, 1);
}

void FFloatProperty::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<float>(*this, Object, 1);
}

void FDoubleProperty::DisplayInImGui(UObject* Object) const
{
    FNumericProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<double>(*this, Object, 1);
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

    char Buffer[IMGUI_FSTRING_BUFFER_SIZE];
    FCStringAnsi::Strncpy(Buffer, Data->ToAnsiString().c_str(), IMGUI_FSTRING_BUFFER_SIZE);
    Buffer[IMGUI_FSTRING_BUFFER_SIZE - 1] = '\0'; // 항상 널 종료 보장

    if (ImGui::InputText(Name, Buffer, IMGUI_FSTRING_BUFFER_SIZE))
    {
        *Data = Buffer;
    }
}

void FNameProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    const FName* Data = GetPropertyData<FName>(Object);
    std::string NameStr = Data->ToString().ToAnsiString();

    // ReadOnly Mode
    ImGui::BeginDisabled(true);
    {
        ImGui::InputText(Name, NameStr.data(), NameStr.size(), ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::EndDisabled();
}

void FVector2DProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<float>(*this, Object, 2);
}

void FVectorProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<float>(*this, Object, 3);
}

void FVector4Property::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<float>(*this, Object, 4);
}

void FRotatorProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<float>(*this, Object, 3);
}

void FQuatProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FPropertyUIHelper::DisplayNumericDragN<float>(*this, Object, 4);
}

void FTransformProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    if (ImGui::TreeNode(Name))
    {
        FTransform* Data = GetPropertyData<FTransform>(Object);
        FRotator Rotation = Data->Rotator();

        FImGuiWidget::DrawVec3Control("Location", Data->Translation);
        FImGuiWidget::DrawRot3Control("Rotation", Rotation);
        FImGuiWidget::DrawVec3Control("Scale", Data->Scale3D, 1.0f);

        Data->Rotation = Rotation.Quaternion();
        ImGui::TreePop();
    }
}

void FMatrixProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    // TODO: 짐벌락 현상 있음
    if (ImGui::TreeNode(Name))
    {
        bool bChanged = false;
        FMatrix* Data = GetPropertyData<FMatrix>(Object);

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

void FColorProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FColor* Data = GetPropertyData<FColor>(Object);
    FLinearColor LinearColor = FLinearColor(*Data);

    constexpr ImGuiColorEditFlags Flags =
        ImGuiColorEditFlags_DisplayRGB
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_AlphaPreviewHalf;

    if (ImGui::ColorEdit4(Name, reinterpret_cast<float*>(&LinearColor), Flags))
    {
        *Data = LinearColor.ToColorRawRGB8();
    }
}

void FLinearColorProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    FLinearColor* Data = GetPropertyData<FLinearColor>(Object);

    constexpr ImGuiColorEditFlags Flags =
        ImGuiColorEditFlags_Float
        | ImGuiColorEditFlags_AlphaBar
        | ImGuiColorEditFlags_AlphaPreview
        | ImGuiColorEditFlags_AlphaPreviewHalf;

    ImGui::ColorEdit4(Name, reinterpret_cast<float*>(Data), Flags);
}

void FSubclassOfProperty::DisplayInImGui(UObject* Object) const
{
    FProperty::DisplayInImGui(Object);

    TSubclassOf<UObject>* Data = GetPropertyData<TSubclassOf<UObject>>(Object);
    UClass* CurrentClass = GetSpecificClass();
    if (CurrentClass == nullptr)
    {
        return;
    }

    TArray<UClass*> ChildClasses;
    GetChildOfClass(CurrentClass, ChildClasses);

    const std::string CurrentClassName = (*Data) ? (*Data)->GetName().ToAnsiString() : "None";
    if (ImGui::BeginCombo(Name, CurrentClassName.c_str()))
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

void FUnresolvedPtrProperty::DisplayInImGui(UObject* Object) const
{
    if (Type == EPropertyType::Object)
    {
        FObjectBaseProperty::DisplayInImGui(Object);
    }
}
