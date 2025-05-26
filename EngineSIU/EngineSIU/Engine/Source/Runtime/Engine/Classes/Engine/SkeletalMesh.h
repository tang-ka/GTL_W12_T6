#pragma once
#include "SkinnedAsset.h"
#include "Asset/SkeletalMeshAsset.h" 

class USkeleton;
class UPhysicsAsset;

class USkeletalMesh : public USkinnedAsset
{
    DECLARE_CLASS(USkeletalMesh, USkinnedAsset)

public:
    USkeletalMesh();
    virtual ~USkeletalMesh() override = default;

    void SetRenderData(std::unique_ptr<FSkeletalMeshRenderData> InRenderData);

    const FSkeletalMeshRenderData* GetRenderData() const;

    USkeleton* GetSkeleton() const { return Skeleton; }
    void SetSkeleton(USkeleton* InSkeleton) { Skeleton = InSkeleton; }

    UPhysicsAsset* GetPhysicsAsset() const { return PhysicsAsset; }
    void SetPhysicsAsset(UPhysicsAsset* InPhysicsAsset) { PhysicsAsset = InPhysicsAsset; }

    virtual void SerializeAsset(FArchive& Ar) override;

protected:
    std::unique_ptr<FSkeletalMeshRenderData> RenderData;

    USkeleton* Skeleton;

    UPhysicsAsset* PhysicsAsset;
};
