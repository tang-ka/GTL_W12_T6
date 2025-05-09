#pragma once
#include <string_view>

/**
 * 타입을 문자열화 합니다.
 * @tparam T 문자열화 할 타입 이름
 * @return 문자열화 된 타입
 *
 * @warning string_view는 직접 값을 바꾸는게 아닌 data section에 있는 __FUNCSIG__문자열을 가리키기 때문에, view.data()로 접근하면 추가적인 문자열이 들어갈 수 있습니다.
 *          이때는 .data()와 .size()를 이용해서 직접 포인터 연산을 하거나, std::string으로 변환해서 사용하면 정상적으로 사용할 수 있습니다.
 */
template <typename T>
constexpr std::string_view GetTypeName()
{
#if defined(__clang__)
    constexpr std::string_view Signature = __PRETTY_FUNCTION__;
    constexpr std::string_view Prefix = "std::string_view GetTypeName() [T = ";
    constexpr std::string_view Suffix = "]";
#elif defined(__GNUC__)
    constexpr std::string_view Signature = __PRETTY_FUNCTION__;
    constexpr std::string_view Prefix = "constexpr std::string_view GetTypeName() [with T = ";
    constexpr std::string_view Suffix = "]";
#elif defined(_MSC_VER)
    constexpr std::string_view Signature = __FUNCSIG__;
    constexpr std::string_view Prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl GetTypeName<";
    constexpr std::string_view Suffix = ">(void)";
#else
    return "__UnknownType__";
#endif

    constexpr auto Start = Signature.find(Prefix) + Prefix.size();
    constexpr auto End = Signature.rfind(Suffix);
    constexpr std::string_view Ret = Signature.substr(Start, End - Start);

#if defined(_MSC_VER)
    constexpr std::string_view TypePrefixes[] = { "class ", "struct ", "enum ", "union " };
    for (const std::string_view TypePrefix : TypePrefixes)
    {
        if (Ret.starts_with(TypePrefix))
        {
            return Ret.substr(TypePrefix.size());
        }
    }
#endif
    return Ret;
}
