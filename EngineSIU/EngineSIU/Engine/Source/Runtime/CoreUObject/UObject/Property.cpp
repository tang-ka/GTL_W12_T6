#include "Property.h"
#include "Class.h"


void FProperty::Resolve()
{
    // TODO: 커스텀 구조체도 가능하게 만들기
}

void FObjectBaseProperty::Resolve()
{
    FProperty::Resolve();

    if (std::holds_alternative<FName>(TypeSpecificData))
    {
        const FName& TypeName = std::get<FName>(TypeSpecificData);
        if (UClass** FoundClass = UClass::GetClassMap().Find(TypeName))
        {
            Type = EPropertyType::Object;
            TypeSpecificData = *FoundClass;
            return;
        }
    }

    /* 이론상 도달할 수 없는 코드.
     * 이 함수가 호출 되었다는건, GetTypeName<T>()로직에 문제가 있어, 이름을 잘못 가져왔거나,
     * 그럴 확률은 매우 낮지만, GetPropertyType<T>()에서 Enum값을 잘못 분류할 때 호출됩니다.
     */
    std::unreachable();
}

void FUnresolvedPtrProperty::Resolve()
{
    if (Type == EPropertyType::Object)
    {
        return;
    }

    if (Type == EPropertyType::UnresolvedPointer)
    {
        FObjectBaseProperty::Resolve();
        return;
    }

    Type = EPropertyType::Unknown;
    TypeSpecificData = std::monostate{};
    UE_LOG(ELogLevel::Error, "Unknown Property Type : %s", Name);
}
