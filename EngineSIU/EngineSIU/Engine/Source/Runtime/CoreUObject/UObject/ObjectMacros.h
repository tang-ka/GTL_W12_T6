// ReSharper disable CppClangTidyBugproneMacroParentheses
// ReSharper disable CppClangTidyClangDiagnosticPedantic
#pragma once
#include "Class.h"
#include "UObjectHash.h"
#include "Templates/TypeUtilities.h"

// name을 문자열화 해주는 매크로
#define INLINE_STRINGIFY(name) #name


// 공통 클래스 정의 부분
#define __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
private: \
    TClass(const TClass&) = delete; \
    TClass& operator=(const TClass&) = delete; \
    TClass(TClass&&) = delete; \
    TClass& operator=(TClass&&) = delete; \
    inline static struct TClass##_StaticClassRegistrar_PRIVATE \
    { \
        TClass##_StaticClassRegistrar_PRIVATE() \
        { \
            UClass::GetClassMap().Add(#TClass, ThisClass::StaticClass()); \
            AddClassToChildListMap(ThisClass::StaticClass()); \
        } \
    } TClass##_StaticClassRegistrar_PRIVATE{}; \
public: \
    using Super = TSuperClass; \
    using ThisClass = TClass;


// RTTI를 위한 클래스 매크로
#define DECLARE_CLASS(TClass, TSuperClass) \
    __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ \
            #TClass, \
            static_cast<uint32>(sizeof(TClass)), \
            static_cast<uint32>(alignof(TClass)), \
            TSuperClass::StaticClass(), \
            []() -> UObject* { \
                void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(sizeof(TClass)); \
                ::new (RawMemory) TClass; \
                return static_cast<UObject*>(RawMemory); \
            } \
        }; \
        return &ClassInfo; \
    }

// RTTI를 위한 추상 클래스 매크로
#define DECLARE_ABSTRACT_CLASS(TClass, TSuperClass) \
    __DECLARE_COMMON_CLASS_BODY__(TClass, TSuperClass) \
    static UClass* StaticClass() { \
        static UClass ClassInfo{ \
            #TClass, \
            static_cast<uint32>(sizeof(TClass)), \
            static_cast<uint32>(alignof(TClass)), \
            TSuperClass::StaticClass(), \
            []() -> UObject* { return nullptr; } \
        }; \
        return &ClassInfo; \
    }


#define GET_FIRST_ARG(First, ...) First
#define FIRST_ARG(...) GET_FIRST_ARG(__VA_ARGS__, )

/**
 * UClass에 Property를 등록합니다.
 * @param Type 선언할 타입
 * @param VarName 변수 이름
 * @param ... 기본값
 *
 * Example Code
 * ```
 * UPROPERTY
 * (int, Value, = 10)
 * ```
 */
#define UPROPERTY(Type, VarName, ...) \
    Type VarName FIRST_ARG(__VA_ARGS__); \
    inline static struct VarName##_PropRegistrar_PRIVATE \
    { \
        VarName##_PropRegistrar_PRIVATE() \
        { \
            constexpr int64 Offset = offsetof(ThisClass, VarName); \
            constexpr EPropertyType TypeEnum = GetPropertyType<Type>(); \
            constexpr std::string_view TypeName = GetTypeName<Type>(); \
            FProperty* Property = new FProperty{ ThisClass::StaticClass(), #VarName, TypeEnum, sizeof(Type), Offset }; \
            if (Property && TypeEnum == EPropertyType::UnresolvedPointer) \
            { \
                Property->TypeSpecificData = FName(TypeName.data(), TypeName.size()); \
            } \
            ThisClass::StaticClass()->RegisterProperty(Property); \
        } \
    } VarName##_PropRegistrar_PRIVATE{};
