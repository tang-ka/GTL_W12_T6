#include "Material.h"
#include "UObject/Casts.h"


UObject* UMaterial::Duplicate(UObject* InOuter)
{
    ThisClass* NewMaterial = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewMaterial->MaterialInfo = MaterialInfo;

    return NewMaterial;
}

void UMaterial::SerializeAsset(FArchive& Ar)
{
    MaterialInfo.Serialize(Ar);
}
