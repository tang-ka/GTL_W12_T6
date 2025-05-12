#include "Property.h"

#include "Class.h"


void FProperty::Resolve()
{
    // TODO: 커스텀 구조체도 가능하게 만들기
}

void FUnresolvedPtrProperty::Resolve()
{
    FObjectBaseProperty::Resolve();

    if (Type == EPropertyType::Object)
    {
        return;
    }

    if (Type == EPropertyType::UnresolvedPointer)
    {
        if (std::holds_alternative<FName>(TypeSpecificData))
        {
            const FName& TypeName = std::get<FName>(TypeSpecificData);
            if (UClass** FoundClass = UClass::GetClassMap().Find(TypeName))
            {
                if (UClass* Class = *FoundClass; Class->IsChildOf(UObject::StaticClass()))
                {
                    Type = EPropertyType::Object;
                    TypeSpecificData = Class;
                    return;
                }
            }
        }
    }

    Type = EPropertyType::Unknown;
    TypeSpecificData = std::monostate{};
    UE_LOG(ELogLevel::Error, "Unknown Property Type : %s", Name);
}
