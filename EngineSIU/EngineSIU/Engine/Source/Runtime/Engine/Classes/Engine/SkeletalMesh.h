#pragma once
#include "SkinnedAsset.h"
#include "Asset/StaticMeshAsset.h"

class USkeleton;
struct FSkeletalMeshRenderData;

class USkeletalMesh : public USkinnedAsset
{
    DECLARE_CLASS(USkeletalMesh, USkinnedAsset)

public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override;

    void SetRenderData(std::unique_ptr<FSkeletalMeshRenderData> InRenderData);

    const FSkeletalMeshRenderData* GetRenderData() const;

    USkeleton* GetSkeleton() const { return Skeleton; }

    void SetSkeleton(USkeleton* InSkeleton) { Skeleton = InSkeleton; }

protected:
    std::unique_ptr<FSkeletalMeshRenderData> RenderData;

    USkeleton* Skeleton;
};
