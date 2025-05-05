
#include "Skeleton.h"

int32 USkeleton::FindBoneIndex(const FName& BoneName) const
{
    return ReferenceSkeleton.FindBoneIndex(BoneName);
}
