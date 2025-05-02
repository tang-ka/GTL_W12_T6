#pragma once
#include "SkinnedAsset.h"

class USkeletalMesh : public USkinnedAsset
{
    DECLARE_CLASS(USkeletalMesh, USkinnedAsset)

public:
    USkeletalMesh() = default;
    virtual ~USkeletalMesh() override = default;
};
