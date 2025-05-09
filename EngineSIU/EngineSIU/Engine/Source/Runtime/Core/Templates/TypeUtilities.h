#pragma once
#include <string_view>

/**
 * 타입을 문자열화 합니다.
 * @tparam T 문자열화 할 타입 이름
 * @return 문자열화 된 타입
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
    return Ret;
#else
    return Ret;
#endif
}
