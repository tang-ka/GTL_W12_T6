#pragma once

#include "BillboardRenderPass.h"

class FWorldBillboardRenderPass : public FBillboardRenderPass
{
public:
    FWorldBillboardRenderPass();
    virtual ~FWorldBillboardRenderPass() override = default;

    virtual void PrepareRenderArr() override;
};
