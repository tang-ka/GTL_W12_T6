#include "GizmoCircleComponent.h"

#define DISC_RESOLUTION 128


bool UGizmoCircleComponent::IntersectsRay(const FVector& RayOrigin, const FVector& RayDir, float& Dist)
{
    if (FMath::IsNearlyZero(RayDir.Y))
    {
        return false; // normal to normal vector of plane
    }

    Dist = -RayOrigin.Y / RayDir.Y;

    const FVector IntersectionPoint = RayOrigin + RayDir * Dist;
    const float IntersectionToDiscCenterSquared = IntersectionPoint.Length();

    return (Inner * Inner < IntersectionToDiscCenterSquared && IntersectionToDiscCenterSquared < 1);
}
