#pragma once
#include "GizmoBaseComponent.h"


class UGizmoCircleComponent : public UGizmoBaseComponent
{
    DECLARE_CLASS(UGizmoCircleComponent, UGizmoBaseComponent)

public:
    UGizmoCircleComponent() = default;

    virtual bool IntersectsRay(const FVector& RayOrigin, const FVector& RayDir, float& Dist);

    float GetInnerRadius() const { return Inner; }
    void SetInnerRadius(float Value) { Inner = Value; }

private:
    float Inner = 1.0f;
};
