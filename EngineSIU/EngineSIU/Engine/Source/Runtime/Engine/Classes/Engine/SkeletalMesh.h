#pragma once
#include "SkinnedAsset.h"

struct FSkeletalMeshRenderData;

class USkeletalMesh : public USkinnedAsset
{
    DECLARE_CLASS(USkeletalMesh, USkinnedAsset)

public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override;

    void SetData(std::unique_ptr<FSkeletalMeshRenderData> InRenderData);

protected:
    std::unique_ptr<FSkeletalMeshRenderData> RenderData;
};
