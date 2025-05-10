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
constexpr std::string_view GetTypeName(bool bIsNameOnly = true)
{
#if defined(__clang__)
    constexpr std::string_view SignatureValue = __PRETTY_FUNCTION__;
    constexpr std::string_view ActualPrefixPattern = " T = ";
    constexpr auto PrefixStartPos = SignatureValue.find(ActualPrefixPattern);
    if (PrefixStartPos == std::string_view::npos) return "__UnknownType_Clang_PrefixFail__";
    constexpr size_t ActualNameStartPosition = PrefixStartPos + ActualPrefixPattern.size();

    constexpr std::string_view SuffixString = "]";
    constexpr auto EndPosition = SignatureValue.rfind(SuffixString);
    if (EndPosition == std::string_view::npos || EndPosition <= ActualNameStartPosition) return "__UnknownType_Clang_SuffixFail__";

    std::string_view RawTypeNameString = SignatureValue.substr(ActualNameStartPosition, EndPosition - ActualNameStartPosition);

    if (!bIsNameOnly)
    {
        constexpr auto TrimWhitespaceLight = [](std::string_view Sv) -> std::string_view {
            if (Sv.empty()) return Sv;
            size_t First = 0;
            while (First < Sv.length() && (Sv[First] == ' ' || Sv[First] == '\t')) {
                First++;
            }
            size_t Last = Sv.length();
            while (Last > First && (Sv[Last - 1] == ' ' || Sv[Last - 1] == '\t')) {
                Last--;
            }
            return Sv.substr(First, Last - First);
        };
        return TrimWhitespaceLight(RawTypeNameString);
    }
    // Clang의 경우에도 네임스페이스 제거가 필요하다면 여기에 로직 추가 가능
    // 예: auto last_colon = RawTypeNameString.rfind("::");
    //     if (last_colon != std::string_view::npos) {
    //         RawTypeNameString = RawTypeNameString.substr(last_colon + 2);
    //     }
    return RawTypeNameString;


#elif defined(__GNUC__)
    constexpr std::string_view SignatureValue = __PRETTY_FUNCTION__;
    constexpr std::string_view ActualPrefixPattern = " T = ";
    constexpr auto PrefixStartPos = SignatureValue.find(ActualPrefixPattern);
    if (PrefixStartPos == std::string_view::npos) return "__UnknownType_GCC_PrefixFail__";

    constexpr size_t ActualNameStartPosition = PrefixStartPos + ActualPrefixPattern.size();

    constexpr std::string_view SuffixString = "]";
    constexpr auto EndPosition = SignatureValue.rfind(SuffixString);
    if (EndPosition == std::string_view::npos || EndPosition <= ActualNameStartPosition) return "__UnknownType_GCC_SuffixFail__";

    std::string_view RawTypeNameString = SignatureValue.substr(ActualNameStartPosition, EndPosition - ActualNameStartPosition);

    if (!bIsNameOnly)
    {
        constexpr auto TrimWhitespaceLight = [](std::string_view Sv) -> std::string_view {
             if (Sv.empty()) return Sv;
            size_t First = 0;
            while (First < Sv.length() && (Sv[First] == ' ' || Sv[First] == '\t')) {
                First++;
            }
            size_t Last = Sv.length();
            while (Last > First && (Sv[Last - 1] == ' ' || Sv[Last - 1] == '\t')) {
                Last--;
            }
            return Sv.substr(First, Last - First);
        };
        return TrimWhitespaceLight(RawTypeNameString);
    }
    return RawTypeNameString;

#elif defined(_MSC_VER)
    constexpr std::string_view FullSignature = __FUNCSIG__;

    // GetTypeName< 앞부분 찾기
    constexpr std::string_view GetTypeNameMarker = "GetTypeName<";
    constexpr auto TypeNameStartMarkerPos = FullSignature.find(GetTypeNameMarker);
    constexpr size_t ActualTypeNameStartPosition = TypeNameStartMarkerPos + GetTypeNameMarker.size();

    // >( 파라미터 시작 전의 꺾쇠 닫기
    // 또는 마지막 > 를 찾고, 그게 파라미터 리스트의 닫는 괄호인지 확인
    // MSVC에서 템플릿 인자가 여러 개인 경우(Foo<Bar<int>, double>) 가장 바깥쪽 꺽쇠를 찾아야 합니다.
    // __FUNCSIG__ 에서 타입 부분만 정확히 추출하는 것은 매우 까다로울 수 있습니다.
    // 여기서는 단순한 경우를 가정하여 마지막 '>'를 찾고 그 이후에 ' ('가 오는지 확인합니다.

    size_t AngleBracketDepth = 0;
    size_t TypeEndPosition = ActualTypeNameStartPosition;
    bool FoundValidEnd = false;
    for (size_t i = ActualTypeNameStartPosition; i < FullSignature.length(); ++i)
    {
        if (FullSignature[i] == '<')
        {
            AngleBracketDepth++;
        }
        else if (FullSignature[i] == '>')
        {
            if (AngleBracketDepth == 0)
            {
                // 가장 바깥쪽 닫는 꺽쇠
                TypeEndPosition = i;
                // 이 꺽쇠 다음에 바로 함수 파라미터 시작 '('가 오는지 확인
                // 또는 함수 시그니처의 끝을 나타내는 SuffixString ("(bool)" 등)으로 확인
                if (i + 1 < FullSignature.length() && FullSignature[i + 1] == '(')
                {
                    // 예: ">("
                    FoundValidEnd = true;
                    break;
                }
                else if (FullSignature.substr(i).starts_with(">(bool)"))
                {
                    // 더 구체적인 패턴
                    FoundValidEnd = true;
                    break;
                }
            }
            else
            {
                AngleBracketDepth--;
            }
        }
    }
    if (!FoundValidEnd)
    {
        // 가장 바깥쪽 닫는 꺾쇠를 찾지 못했거나, 그 뒤 패턴이 안 맞으면
        // 대체 로직: 마지막 '>'를 찾는다.
        // 이는 중첩 템플릿 (예: MyType<OtherType<int>>)에서 내부 템플릿의 '>'를 잡을 위험이 있음.
        // __FUNCSIG__ 구조에 대한 강한 가정이 필요함.
        // 일반적으로 GetTypeName<...TYPE...>(bool...) 형태일 것이므로,
        // 마지막 '>'가 타입의 끝일 가능성이 높습니다.
        auto TempEndPos = FullSignature.rfind('>');
        // 그 '>'가 함수 파라미터 '(' 앞에 있는지 확인
        auto ParamOpenParen = FullSignature.find('(', ActualTypeNameStartPosition);
        if (TempEndPos != std::string_view::npos && ParamOpenParen != std::string_view::npos && TempEndPos <
            ParamOpenParen)
        {
            TypeEndPosition = TempEndPos;
        }
        else if (TempEndPos != std::string_view::npos)
        {
            // 백업으로 마지막 '>' 사용
            TypeEndPosition = TempEndPos;
        }
        else
        {
            return "__UnknownType_MSVC_SuffixFail__";
        }
    }


    std::string_view RawTypeNameString = FullSignature.substr(
        ActualTypeNameStartPosition, TypeEndPosition - ActualTypeNameStartPosition
    );

    constexpr auto TrimWhitespace = [](std::string_view Sv) -> std::string_view
    {
        if (Sv.empty())
        {
            return Sv;
        }
        const char* WhitespaceChars = " \t\n\r\f\v";
        auto FirstNonWhitespace = Sv.find_first_not_of(WhitespaceChars);
        if (std::string_view::npos == FirstNonWhitespace)
        {
            return {};
        }
        auto LastNonWhitespace = Sv.find_last_not_of(WhitespaceChars);
        return Sv.substr(FirstNonWhitespace, LastNonWhitespace - FirstNonWhitespace + 1);
    };

    // constexpr로 알파벳/숫자인지 확인하는 헬퍼 람다
    constexpr auto IsAlnumChar = [](char C) -> bool
    {
        return (C >= 'a' && C <= 'z') ||
            (C >= 'A' && C <= 'Z') ||
            (C >= '0' && C <= '9');
    };

    if (!bIsNameOnly)
    {
        return TrimWhitespace(RawTypeNameString);
    }

    std::string_view ProcessedTypeNameString = TrimWhitespace(RawTypeNameString);
    bool WasModified;

    // 후행 한정자 (포인터, 참조, cv-한정자) 제거
    do
    {
        WasModified = false;
        std::string_view TempTrimmedString = TrimWhitespace(ProcessedTypeNameString);

        if (TempTrimmedString.ends_with("&&"))
        {
            ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 2);
            WasModified = true;
        }
        else if (TempTrimmedString.ends_with("&"))
        {
            ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 1);
            WasModified = true;
        }
        else if (TempTrimmedString.ends_with("*"))
        {
            ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 1);
            WasModified = true;
        }
        else if (TempTrimmedString.length() >= 6 && TempTrimmedString.substr(TempTrimmedString.length() - 6) ==
            " const")
        {
            ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 6);
            WasModified = true;
        }
        else if (TempTrimmedString.length() >= 5 && TempTrimmedString.substr(TempTrimmedString.length() - 5) == "const")
        {
            if (TempTrimmedString.length() == 5 ||
                (TempTrimmedString.length() > 5 && !IsAlnumChar(TempTrimmedString[TempTrimmedString.length() - 6])))
            {
                ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 5);
                WasModified = true;
            }
        }
        else if (TempTrimmedString.length() >= 9 && TempTrimmedString.substr(TempTrimmedString.length() - 9) ==
            " volatile")
        {
            ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 9);
            WasModified = true;
        }
        else if (TempTrimmedString.length() >= 8 && TempTrimmedString.substr(TempTrimmedString.length() - 8) ==
            "volatile")
        {
            if (TempTrimmedString.length() == 8 ||
                (TempTrimmedString.length() > 8 && !IsAlnumChar(TempTrimmedString[TempTrimmedString.length() - 9])))
            {
                ProcessedTypeNameString = TempTrimmedString.substr(0, TempTrimmedString.length() - 8);
                WasModified = true;
            }
        }

        if (WasModified)
        {
            ProcessedTypeNameString = TrimWhitespace(ProcessedTypeNameString);
        }
    }
    while (WasModified);

    // 선행 cv-한정자 제거
    do
    {
        WasModified = false;
        std::string_view TempTrimmedString = TrimWhitespace(ProcessedTypeNameString);

        if (TempTrimmedString.length() >= 6 && TempTrimmedString.substr(0, 6) == "const ")
        {
            ProcessedTypeNameString = TempTrimmedString.substr(6);
            WasModified = true;
        }
        else if (TempTrimmedString.length() >= 5 && TempTrimmedString.substr(0, 5) == "const")
        {
            if (TempTrimmedString.length() == 5 ||
                (TempTrimmedString.length() > 5 && !IsAlnumChar(TempTrimmedString[5])))
            {
                ProcessedTypeNameString = TempTrimmedString.substr(5);
                WasModified = true;
            }
        }
        else if (TempTrimmedString.length() >= 9 && TempTrimmedString.substr(0, 9) == "volatile ")
        {
            ProcessedTypeNameString = TempTrimmedString.substr(9);
            WasModified = true;
        }
        else if (TempTrimmedString.length() >= 8 && TempTrimmedString.substr(0, 8) == "volatile")
        {
            if (TempTrimmedString.length() == 8 ||
                (TempTrimmedString.length() > 8 && !IsAlnumChar(TempTrimmedString[8])))
            {
                ProcessedTypeNameString = TempTrimmedString.substr(8);
                WasModified = true;
            }
        }

        if (WasModified)
        {
            ProcessedTypeNameString = TrimWhitespace(ProcessedTypeNameString);
        }
    }
    while (WasModified);

    // 선행 타입 키워드 ("class ", "struct ", "enum ", "union ") 제거
    constexpr std::string_view TypePrefixKeywords[] = {"class ", "struct ", "enum ", "union "};
    do
    {
        WasModified = false;
        std::string_view TempTrimmedString = TrimWhitespace(ProcessedTypeNameString);
        for (const std::string_view KeywordPrefix : TypePrefixKeywords)
        {
            if (TempTrimmedString.starts_with(KeywordPrefix))
            {
                ProcessedTypeNameString = TempTrimmedString.substr(KeywordPrefix.size());
                WasModified = true;
                break;
            }
        }
        if (WasModified)
        {
            ProcessedTypeNameString = TrimWhitespace(ProcessedTypeNameString);
        }
    }
    while (WasModified);

    // 네임스페이스 제거 (bIsNameOnly == true 인 경우에만 적용)
    // 이 로직은 다른 모든 정제 작업 (cv, ptr/ref, class/struct 키워드) 이후에 적용됩니다.
    std::string_view FinalTypeNameString = TrimWhitespace(ProcessedTypeNameString);
    auto LastColonColonPos = FinalTypeNameString.rfind("::");
    if (LastColonColonPos != std::string_view::npos)
    {
        // "::" 뒤에 실제 이름이 있는지 확인 (예: "MyNamespace::"와 같은 경우 방지)
        if (LastColonColonPos + 2 < FinalTypeNameString.length())
        {
            FinalTypeNameString = FinalTypeNameString.substr(LastColonColonPos + 2);
        }
    }

    return FinalTypeNameString;

#else
    return "__UnknownType__";
#endif
}
