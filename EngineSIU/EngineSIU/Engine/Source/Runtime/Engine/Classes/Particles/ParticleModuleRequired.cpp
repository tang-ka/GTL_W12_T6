#include "ParticleModuleRequired.h"

void UParticleModuleRequired::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}

FParticleRequiredModule* UParticleModuleRequired::CreateRendererResource()
{
    FParticleRequiredModule *FReqMod = new FParticleRequiredModule();
    // FReqMod->bCutoutTexureIsValid = IsBoundingGeometryValid(); // TODO: 필요한 거 주석 해제
    // FReqMod->bUseVelocityForMotionBlur = ShouldUseVelocityForMotionBlur();
    // FReqMod->NumFrames = GetNumFrames();
    // FReqMod->FrameData = DerivedData.BoundingGeometry;
    // FReqMod->NumBoundingVertices = GetNumBoundingVertices();
    // FReqMod->NumBoundingTriangles = GetNumBoundingTriangles();
    // check(FReqMod->NumBoundingTriangles == 2 || FReqMod->NumBoundingTriangles == 6);
    // FReqMod->AlphaThreshold = AlphaThreshold;
    // FReqMod->BoundingGeometryBufferSRV = GetBoundingGeometrySRV();
    return FReqMod;
}
