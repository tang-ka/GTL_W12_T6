#pragma once
#include "CoreMiscDefines.h"
#include "Container/Array.h"
#include "Container/Map.h"
#include "UObject/NameTypes.h"

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
private:
    TArray<FMeshBoneInfo> RawRefBoneInfo;
    TArray<FTransform> RawRefBonePose;

    TArray<FMeshBoneInfo> FinalRefBoneInfo;
    TArray<FTransform> FinalRefBonePose;

    TMap<FName, int32> RawNameToIndexMap;
    TMap<FName, int32> FinalNameToIndexMap;

    TArray<FBoneIndexType>  RequiredVirtualBones;
    TArray<FVirtualBoneRefData> UsedVirtualBoneData;
};
