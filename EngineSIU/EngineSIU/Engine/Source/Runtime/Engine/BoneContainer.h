#pragma once
#include "Container/Array.h"
#include "HAL/PlatformType.h"
#include "Animation/Skeleton.h"


struct FReferenceSkeleton;
class USkeletalMesh;
class USkeleton;

struct FSkelMeshRefPoseOverride
{
    /** Inverse of (component space) ref pose matrices  */
    TArray<FMatrix> RefBasesInvMatrix;
    /** Per bone transforms (local space) for new ref pose */
    TArray<FTransform> RefBonePoses;
};

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

    std::shared_ptr<FSkelMeshRefPoseOverride> RefPoseOverride;

public:
    FBoneContainer();

    USkeletalMesh* GetSkeletalMeshAsset() const
    {
        return AssetSkeletalMesh;
    }

    USkeleton* GetSkeletonAsset(bool bEvenIfUnreachable = false) const
    {
        return AssetSkeleton;
    }
    
    // Fill the supplied buffer with the compact pose reference pose
    template<typename ArrayType>
    void FillWithCompactRefPose(ArrayType& OutTransforms) const
    {
        const int32 CompactPoseBoneCount = GetCompactPoseNumBones();
        OutTransforms.Reserve(CompactPoseBoneCount);
        if (RefPoseOverride)
        {
            OutTransforms.Append(RefPoseOverride->RefBonePoses);
        }
        else
        {
            OutTransforms.SetNum(CompactPoseBoneCount);
            const TArray<FTransform>& RefPoseTransforms = RefSkeleton->GetRawRefBonePose();
            for (int32 CompactBoneIndex = 0; CompactBoneIndex < CompactPoseBoneCount; ++CompactBoneIndex)
            {
                OutTransforms[CompactBoneIndex] = RefPoseTransforms[BoneIndices[CompactBoneIndex]];
            }
        }
    }

    const FTransform& GetRefPoseTransform(const int32& BoneIndex) const
    {
        if (RefPoseOverride)
        {
            return RefPoseOverride->RefBonePoses[BoneIndex];
        }
        else
        {
            return RefSkeleton->GetRawRefBonePose()[BoneIndices[BoneIndex]];
        }
    }
    
    const bool IsValid() const
    {
        return (Asset && (RefSkeleton != nullptr) && (BoneIndices.Num() > 0));
    }

    const TArray<uint16>& GetBoneIndicesArray() const
    {
        return BoneIndices;
    }

    const int32 GetCompactPoseNumBones() const
    {
        return BoneIndices.Num();
    }

    void SetRefPoseOverride(const std::shared_ptr<FSkelMeshRefPoseOverride>& InRefPoseOverride)
    {
        if (InRefPoseOverride.get() != RefPoseOverride.get())
        {
            RefPoseOverride = InRefPoseOverride;
        }
    }

    int32 GetParentBoneIndex(const int32 BoneIndex) const
    {
        return RefSkeleton->GetParentIndex(BoneIndex);
    }

    int32 GetSkeletonIndex(const int32& BoneIndex) const
    {
        return CompactPoseToSkeletonIndex[BoneIndex];
    }
};
