#pragma once
#include "CoreMiscDefines.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "UObject/NameTypes.h"
#include "Math/Transform.h"

struct FMeshBoneInfo
{
    // Bone's name.
    FName Name;

    // INDEX_NONE if this is the root bone. 
    int32 ParentIndex;

    FMeshBoneInfo() : Name(NAME_None), ParentIndex(INDEX_NONE) {}

    FMeshBoneInfo(const FName& InName, int32 InParentIndex)
        : Name(InName)
        , ParentIndex(InParentIndex)
    {}

    bool operator==(const FMeshBoneInfo& B) const
    {
        return(Name == B.Name);
    }
};

struct FReferenceSkeleton
{
public:
    TArray<FMeshBoneInfo> RawRefBoneInfo;
    TArray<FTransform> RawRefBonePose;

    TMap<FName, int32> RawNameToIndexMap;

    int32 FindBoneIndex(const FName& BoneName) const;
};
