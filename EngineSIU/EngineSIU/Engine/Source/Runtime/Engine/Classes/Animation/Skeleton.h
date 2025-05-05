#pragma once
#include "Math/Quat.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "ReferenceSkeleton.h"

class USkeleton : public UObject
{
    DECLARE_CLASS(USkeleton, UObject)

public:
    USkeleton() = default;
    virtual ~USkeleton() override = default;

    const FReferenceSkeleton& GetReferenceSkeleton() const
    {
        return ReferenceSkeleton;
    }

    void SetReferenceSkeleton(const FReferenceSkeleton& InReferenceSkeleton)
    {
        ReferenceSkeleton = InReferenceSkeleton;
    }

    int32 FindBoneIndex(const FName& BoneName) const;

protected:
    FReferenceSkeleton ReferenceSkeleton;
    
};
