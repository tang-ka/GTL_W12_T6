#pragma once
#include <variant>
#include <optional>

#include "Object.h"
#include "PropertyTypes.h"
#include "Templates/TypeUtilities.h"


struct FProperty
{
    /**
     * 클래스의 프로퍼티를 등록합니다.
     * @param InOwnerClass 이 프로퍼티를 가지고 있는 UClass
     * @param InPropertyName 프로퍼티의 이름
     * @param InType 프로퍼티의 타입
     * @param InSize 프로퍼티의 크기
     * @param InOffset 프로퍼티가 클래스로부터 떨어진 거리
     * @param InFlags Reflection 관련 Flag
     */
    FProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        EPropertyType InType,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : OwnerClass(InOwnerClass)
        , Name(InPropertyName)
        , Type(InType)
        , Size(InSize)
        , Offset(InOffset)
        , Flags(InFlags)
    {
    }

    virtual ~FProperty() = default;
    FProperty(const FProperty&) = default;
    FProperty& operator=(const FProperty&) = default;
    FProperty(FProperty&&) = default;
    FProperty& operator=(FProperty&&) = default;

public:
    /** ImGui에 각 프로퍼티에 맞는 UI를 띄웁니다. */
    virtual void DisplayInImGui(UObject* Object) const;

    /** 런타임에 타입 정보를 검사하고 업데이트 합니다. */
    virtual void Resolve(); // TODO: 자식 FProperty 만들면 사용

    /**
     * 이 프로퍼티가 EPropertyType::Object 타입일 경우, 해당 UObject의 UClass*를 반환합니다.
     * 그렇지 않거나 아직 해결되지 않은 경우 nullptr을 반환합니다.
     * @return UClass* 또는 nullptr
     */
    FORCEINLINE UClass* GetSpecificClass() const
    {
        return GetTypeSpecificDataAs<UClass*>().value_or(nullptr);
    }

private:
    /**
     * TypeSpecificData에서 특정 타입 T의 값을 안전하게 가져옵니다.
     * 만약 현재 저장된 타입이 T가 아니거나, Type이 해당 T를 가질 수 있는
     * EPropertyType 상태가 아니라면 std::nullopt를 반환합니다.
     *
     * @tparam T 가져오고자 하는 타입 (예: UClass*, FStructInfo*, FName)
     * @return 해당 타입의 값을 담은 std::optional<T>, 또는 std::nullopt
     */
    template <typename T>
    [[nodiscard]] std::optional<T> GetTypeSpecificDataAs() const
    {
        if constexpr (std::same_as<T, UClass*>)
        {
            if (Type != EPropertyType::Object)
            {
                // Object 타입일 때만 UClass*가 유효
                return std::nullopt;
            }
        }
        // else if constexpr (std::same_as<T, FStructInfo*>) {
        //     if (Type != EPropertyType::Struct) { // Struct 타입일 때만 FStructInfo*가 유효
        //         return std::nullopt;
        //     }
        // }
        else if constexpr (std::same_as<T, FName>)
        {
            if (Type != EPropertyType::UnresolvedPointer /* && Type != EPropertyType::UnresolvedStruct 등 */)
            {
                return std::nullopt;
            }
        }

        // 실제 variant에서 값 가져오기 시도
        if (std::holds_alternative<T>(TypeSpecificData))
        {
            return std::get<T>(TypeSpecificData);
        }
        return std::nullopt;
    }

protected:
    /**
     * Property를 실제 데이터 타입으로 변환합니다.
     * @tparam T 변환할 타입
     * @param Object 변환할 값을 가지고 있는 Object
     * @return Object의 실제 값
     *
     * @warning 타입이 잘못되면 UB가 발생할 수 있습니다.
     */
    template <typename T>
    T* GetPropertyData(UObject* Object) const
    {
        // assert(GetPropertyType<T>() == Type); // TODO: UnresolvedType 해결하기
        return reinterpret_cast<T*>(reinterpret_cast<std::byte*>(Object) + Offset);
    }

private:
    friend struct FPropertyUIHelper;

public:
    UClass* OwnerClass;

    const char* Name;
    EPropertyType Type;
    int64 Size;
    int64 Offset;
    EPropertyFlags Flags;

public:
    std::variant<
        std::monostate,  // 현재 값이 없음을 나타냄
        UClass*,         // FProperty의 Type이 Object일때 원본 Property를 가지고 있는 UClass
        // FStructInfo*,    // FProperty의 Type이 Struct일때 커스텀 구조체의 정보
        FName            // FProperty의 Type이 UnresolvedPointer일 때 런타임에 검사할 UClass 이름
    > TypeSpecificData;
};


struct FNumericProperty : public FProperty
{
    FNumericProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        EPropertyType InType,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, InType, InSize, InOffset, InFlags)
    {
    }
};

struct FInt8Property : public FNumericProperty
{
    FInt8Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int8, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FInt16Property : public FNumericProperty
{
    FInt16Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int16, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FInt32Property : public FNumericProperty
{
    FInt32Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int32, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FInt64Property : public FNumericProperty
{
    FInt64Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Int64, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FUInt8Property : public FNumericProperty
{
    FUInt8Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt8, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FUInt16Property : public FNumericProperty
{
    FUInt16Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt16, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FUInt32Property : public FNumericProperty
{
    FUInt32Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt32, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FUInt64Property : public FNumericProperty
{
    FUInt64Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::UInt64, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FFloatProperty : public FNumericProperty
{
    FFloatProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Float, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FDoubleProperty : public FNumericProperty
{
    FDoubleProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FNumericProperty(InOwnerClass, InPropertyName, EPropertyType::Double, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FBoolProperty : public FProperty
{
    FBoolProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Bool, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FStrProperty : public FProperty
{
    FStrProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::String, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FNameProperty : public FProperty
{
    FNameProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Name, InSize, InOffset, InFlags)
    {}

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FVector2DProperty : public FProperty
{
    FVector2DProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Vector2D, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FVectorProperty : public FProperty
{
    FVectorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Vector, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FVector4Property : public FProperty
{
    FVector4Property(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Vector4, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FRotatorProperty : public FProperty
{
    FRotatorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Rotator, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FQuatProperty : public FProperty
{
    FQuatProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Quat, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FTransformProperty : public FProperty
{
    FTransformProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Transform, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FMatrixProperty : public FProperty
{
    FMatrixProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Matrix, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FColorProperty : public FProperty
{
    FColorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Color, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FLinearColorProperty : public FProperty
{
    FLinearColorProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::LinearColor, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};


struct FArrayProperty : public FProperty
{
    FArrayProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Array, InSize, InOffset, InFlags)
    {
    }
};

struct FMapProperty : public FProperty
{
    FMapProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Map, InSize, InOffset, InFlags)
    {
    }
};

struct FSetProperty : public FProperty
{
    FSetProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Set, InSize, InOffset, InFlags)
    {
    }
};

template <typename InEnumType>
struct TEnumProperty : public FProperty
{
    using EnumType = InEnumType;

    TEnumProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Enum, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override
    {
        FProperty::DisplayInImGui(Object);

        EnumType* Data = GetPropertyData<EnumType>(Object);
        constexpr auto EnumEntries = magic_enum::enum_entries<EnumType>();

        auto CurrentNameOpt = magic_enum::enum_name(*Data);
        const std::string CurrentName = CurrentNameOpt.value_or("Unknown");

        if (ImGui::BeginCombo(Name, CurrentName.c_str()))
        {
            for (const auto& [Enum, NameView] : EnumEntries)
            {
                const std::string EnumName = NameView;
                const bool bIsSelected = (*Data == Enum);
                if (ImGui::Selectable(EnumName.c_str(), bIsSelected))
                {
                    *Data = Enum;
                }
                if (bIsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
};

struct FObjectBaseProperty : public FProperty
{
    FObjectBaseProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        EPropertyType InType,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, InType, InSize, InOffset, InFlags)
    {
    }
};

struct FObjectProperty : public FObjectBaseProperty
{
    FObjectProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FObjectBaseProperty(InOwnerClass, InPropertyName, EPropertyType::Object, InSize, InOffset, InFlags)
    {
    }
};

struct FUnresolvedPtrProperty : public FObjectBaseProperty
{
    FUnresolvedPtrProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FObjectBaseProperty(InOwnerClass, InPropertyName, EPropertyType::UnresolvedPointer, InSize, InOffset, InFlags)
    {
    }

    virtual void DisplayInImGui(UObject* Object) const override;
};

struct FStructProperty : public FProperty
{
    FStructProperty(
        UClass* InOwnerClass,
        const char* InPropertyName,
        int32 InSize,
        int32 InOffset,
        EPropertyFlags InFlags = EPropertyFlags::None
    )
        : FProperty(InOwnerClass, InPropertyName, EPropertyType::Struct, InSize, InOffset, InFlags)
    {
    }
};


// struct FDelegateProperty : public FProperty {};  // TODO: 나중에 Delegate Property 만들기

// struct FMulticastDelegateProperty : public FProperty {};


template <typename T>
FProperty* MakeProperty(
    UClass* InOwnerClass,
    const char* InPropertyName,
    int32 InOffset,
    EPropertyFlags InFlags = EPropertyFlags::None
)
{
    constexpr EPropertyType TypeEnum = GetPropertyType<T>();

    if constexpr      (TypeEnum == EPropertyType::Int8)        { return new FInt8Property        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Int16)       { return new FInt16Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Int32)       { return new FInt32Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Int64)       { return new FInt64Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt8)       { return new FUInt8Property       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt16)      { return new FUInt16Property      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt32)      { return new FUInt32Property      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::UInt64)      { return new FUInt64Property      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Float)       { return new FFloatProperty       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Double)      { return new FDoubleProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Bool)        { return new FBoolProperty        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }

    else if constexpr (TypeEnum == EPropertyType::String)      { return new FStrProperty         { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Name)        { return new FNameProperty        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Vector2D)    { return new FVector2DProperty    { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Vector)      { return new FVectorProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Vector4)     { return new FVector4Property     { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Rotator)     { return new FRotatorProperty     { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Quat)        { return new FQuatProperty        { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Transform)   { return new FTransformProperty   { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Matrix)      { return new FMatrixProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Color)       { return new FColorProperty       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::LinearColor) { return new FLinearColorProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }

    else if constexpr (TypeEnum == EPropertyType::Array)       { return new FArrayProperty       { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Map)         { return new FMapProperty         { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Set)         { return new FSetProperty         { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }

    else if constexpr (TypeEnum == EPropertyType::Enum)        { return new TEnumProperty<T>     { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Struct)      { return new FStructProperty      { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags }; }
    else if constexpr (TypeEnum == EPropertyType::Object)
    {
        FProperty* Property = new FObjectProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
        // Property->TypeSpecificData = ; // TODO: PropertyClass 설정
        return Property;
    }
    else if constexpr (TypeEnum == EPropertyType::UnresolvedPointer)
    {
        constexpr std::string_view TypeName = GetTypeName<T>();
        FProperty* Property = new FUnresolvedPtrProperty { InOwnerClass, InPropertyName, sizeof(T), InOffset, InFlags };
        Property->TypeSpecificData = FName(TypeName.data(), TypeName.size());
        return Property;
    }
    else
    {
        static_assert(!std::same_as<T, T>, "Unsupported Property Type"); // 지원되지 않는 타입!!
    }

    std::unreachable(); // 모든 Enum값에 대해서 처리하지 않으면 이 코드가 호출될 수 있음
}
