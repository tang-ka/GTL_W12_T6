// ReSharper disable CppClangTidyCppcoreguidelinesMissingStdForward
// ReSharper disable CppCStyleCast
#pragma once
#include "HAL/PlatformType.h"

template <typename T>
FORCEINLINE constexpr std::remove_reference_t<T>&& MoveTemp(T&& Obj) noexcept
{
    using CastType = std::remove_reference_t<T>;

    // rvalue나 const 객체가 전달되지 않았는지 확인합니다.
    // rvalue는 불필요하고, const 객체는 거의 확실히 실수일 가능성이 있습니다.
    static_assert(std::is_lvalue_reference_v<T>, "MoveTemp called on an rvalue");
    static_assert(!std::same_as<CastType&, const CastType&>, "MoveTemp called on a const object");

    return (CastType&&)Obj;
}
