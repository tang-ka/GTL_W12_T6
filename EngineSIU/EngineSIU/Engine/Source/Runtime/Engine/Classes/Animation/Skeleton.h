#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton : public UObject
{
    DECLARE_CLASS(USkeleton, UObject)

public:
    USkeleton() = default;
    virtual ~USkeleton() override = default;
};
