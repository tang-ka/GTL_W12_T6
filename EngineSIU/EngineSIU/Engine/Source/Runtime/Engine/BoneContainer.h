#pragma once
#include "Container/Array.h"
#include "HAL/PlatformType.h"


struct FReferenceSkeleton;
class USkeletalMesh;
class USkeleton;

struct FBoneContainer
{
private:
    TArray<uint16> BoneIndices;

    UObject* Asset;
    USkeletalMesh* AssetSkeletalMesh;
    USkeleton* AssetSkeleton;

    const FReferenceSkeleton* RefSkeleton;

    TArray<int32> SkeletonToBoneIndexArray;

    TArray<int32> PoseToSkeletonBoneIndexArray;

    TArray<int32> CompactPoseToSkeletonIndex;

    TArray<int32> SkeletonToCompactPose;

    TArray<int32> CompactPoseParentBones;

public:
    FBoneContainer();
};
