
#include "ReferenceSkeleton.h"

#include "Math/Matrix.h"

int32 FReferenceSkeleton::FindBoneIndex(const FName& BoneName) const
{
    if (RawNameToIndexMap.Contains(BoneName))
    {
        return RawNameToIndexMap[BoneName];
    }
    return INDEX_NONE;
}
