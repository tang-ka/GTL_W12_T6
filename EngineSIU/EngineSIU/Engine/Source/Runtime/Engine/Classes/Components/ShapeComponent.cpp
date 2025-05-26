
#include "ShapeComponent.h"

UShapeComponent::UShapeComponent()
{
}

void UShapeComponent::TickComponent(float DeltaTime)
{
    UPrimitiveComponent::TickComponent(DeltaTime);
    // 비물리 충돌 일단 주석 (Overlap)
    // UpdateOverlaps();
}
