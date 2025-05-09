#pragma once

enum class EPropertyType : uint8
{
    Unknown,                       // 알 수 없는 타입
    Int8, Int16, Int32, Int64,     // 부호 있는 정수타입
    UInt8, UInt16, UInt32, UInt64, // 부호 없는 정수타입
    Float, Double,                 // 실수 타입
    Bool,                          // Boolean 타입
    String,                        // 문자열 타입 (FString)
    Name,                          // 이름 타입 (FName)
    Array,                         // TArray<T>
    Map,                           // TMap<T>
    Set,                           // TSet<T>
    Enum,                          // 커스텀 Enum 타입
    Object,                        // UObject* 타입
    Struct,                        // 사용자 정의 구조체 타입
};

template <typename T>
consteval EPropertyType GetPropertyType()
{
    if constexpr (std::is_same_v<T, int8_t>)
        return EPropertyType::Int8;
    else if constexpr (std::is_same_v<T, int16_t>)
        return EPropertyType::Int16;
    else if constexpr (std::is_same_v<T, int32_t>)
        return EPropertyType::Int32;
    else if constexpr (std::is_same_v<T, int64_t>)
        return EPropertyType::Int64;
    else if constexpr (std::is_same_v<T, uint8_t>)
        return EPropertyType::UInt8;
    else if constexpr (std::is_same_v<T, uint16_t>)
        return EPropertyType::UInt16;
    else if constexpr (std::is_same_v<T, uint32_t>)
        return EPropertyType::UInt32;
    else if constexpr (std::is_same_v<T, uint64_t>)
        return EPropertyType::UInt64;
    else if constexpr (std::is_same_v<T, float>)
        return EPropertyType::Float;
    else if constexpr (std::is_same_v<T, double>)
        return EPropertyType::Double;
    else if constexpr (std::is_same_v<T, bool>)
        return EPropertyType::Bool;
    else if constexpr (std::is_same_v<T, FString>)
        return EPropertyType::String;
    else if constexpr (std::is_same_v<T, FName>)
        return EPropertyType::Name;
    // else if constexpr (TIsArray<T>::Value)
    //     return EPropertyType::Array;
    // else if constexpr (TIsMap<T>::Value)
    //     return EPropertyType::Map;
    // else if constexpr (TIsSet<T>::Value)
    //     return EPropertyType::Set;
    else if constexpr (std::is_enum_v<T>)
        return EPropertyType::Enum;
    else if constexpr (std::is_base_of_v<UObject, T>)
        return EPropertyType::Object;
    else if constexpr (std::is_class_v<T>)
        return EPropertyType::Struct;
    return EPropertyType::Unknown;
}

enum class EPropertyFlags : uint32  // NOLINT(performance-enum-size)
{
    None            = 0,         // 플래그 없음
    ImGuiReadOnly   = 1 << 0,    // ImGui에서 읽기 전용으로 표시
    ImGuiReadWrite  = 1 << 1,    // ImGui에서 읽기/쓰기 가능 (기본값으로 사용 가능)
    LuaBindable     = 1 << 2,    // Lua에 자동으로 바인딩
    HiddenInEditor  = 1 << 3,    // 에디터(ImGui)에 표시하지 않음
    // ... 필요한 다른 플래그들 (예: SaveGame, Replicated 등)
};

// 비트 플래그 연산을 위한 헬퍼 함수들
inline EPropertyFlags operator|(EPropertyFlags Lhs, EPropertyFlags Rhs)
{
    return static_cast<EPropertyFlags>(static_cast<uint32_t>(Lhs) | static_cast<uint32_t>(Rhs));
}

inline EPropertyFlags& operator|=(EPropertyFlags& Lhs, EPropertyFlags Rhs)
{
    Lhs = Lhs | Rhs;
    return Lhs;
}

inline bool HasFlag(EPropertyFlags Flags, EPropertyFlags FlagToCheck)
{
    return (static_cast<uint32_t>(Flags) & static_cast<uint32_t>(FlagToCheck)) != 0;
}

// 특정 플래그만 제외하는 연산자
inline EPropertyFlags operator~(EPropertyFlags Flags)
{
    return static_cast<EPropertyFlags>(~static_cast<uint32_t>(Flags));
}

inline EPropertyFlags operator&(EPropertyFlags Lhs, EPropertyFlags Rhs)
{
    return static_cast<EPropertyFlags>(static_cast<uint32_t>(Lhs) & static_cast<uint32_t>(Rhs));
}
