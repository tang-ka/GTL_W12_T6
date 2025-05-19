#pragma once

#include "BillboardRenderPass.h"

class FEditorBillboardRenderPass : public FBillboardRenderPass
{
public:
    FEditorBillboardRenderPass();
    virtual ~FEditorBillboardRenderPass() override = default;

    virtual void PrepareRenderArr() override;
};
