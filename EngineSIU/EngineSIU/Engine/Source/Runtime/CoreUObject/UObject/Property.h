#pragma once
#include <variant>
#include <optional>

#include "Object.h"
#include "PropertyTypes.h"


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
