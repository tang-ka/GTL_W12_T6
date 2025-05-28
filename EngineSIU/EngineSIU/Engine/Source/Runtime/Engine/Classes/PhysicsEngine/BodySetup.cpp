#include "BodySetup.h"
#include "PhysicsEngine/SphereElem.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/SphylElem.h"
#include "PhysicalMaterial.h"
#include <Engine/Asset/StaticMeshAsset.h>
#include "Components/StaticMeshComponent.h"

UBodySetup::UBodySetup()
{
    PhysMaterial = new UPhysicalMaterial;
}

void FKSphereElem::ScaleElem(FVector DeltaSize, float MinSize)
{
    // Find element with largest magnitude, btu preserve sign.
    float DeltaRadius = DeltaSize.X;
    if (FMath::Abs(DeltaSize.Y) > FMath::Abs(DeltaRadius))
        DeltaRadius = DeltaSize.Y;
    else if (FMath::Abs(DeltaSize.Z) > FMath::Abs(DeltaRadius))
        DeltaRadius = DeltaSize.Z;

    Radius = FMath::Max(Radius + DeltaRadius, MinSize);
}

FKSphereElem FKSphereElem::GetFinalScaled(const FVector& Scale3D, const FTransform& RelativeTM) const
{
    float MinScale, MinScaleAbs;
    FVector Scale3DAbs;

    SetupNonUniformHelper(Scale3D * RelativeTM.GetScale3D(), MinScale, MinScaleAbs, Scale3DAbs);

    FKSphereElem ScaledSphere = *this;
    ScaledSphere.Radius *= MinScaleAbs;

    ScaledSphere.Center = RelativeTM.TransformPosition(Center) * Scale3D;


    return ScaledSphere;
}

float FKSphereElem::GetShortestDistanceToPoint(const FVector& WorldPosition, const FTransform& LocalToWorldTM) const
{
    FKSphereElem ScaledSphere = GetFinalScaled(LocalToWorldTM.GetScale3D(), FTransform::Identity);

    const FVector Dir = LocalToWorldTM.TransformPositionNoScale(ScaledSphere.Center) - WorldPosition;
    const float DistToCenter = Dir.Size();
    const float DistToEdge = DistToCenter - ScaledSphere.Radius;

    return DistToEdge > KINDA_SMALL_NUMBER ? DistToEdge : 0.f;
}

float FKSphereElem::GetClosestPointAndNormal(const FVector& WorldPosition, const FTransform& LocalToWorldTM, FVector& ClosestWorldPosition, FVector& Normal) const
{
    FKSphereElem ScaledSphere = GetFinalScaled(LocalToWorldTM.GetScale3D(), FTransform::Identity);

    const FVector Dir = LocalToWorldTM.TransformPositionNoScale(ScaledSphere.Center) - WorldPosition;
    const float DistToCenter = Dir.Size();
    const float DistToEdge = FMath::Max(DistToCenter - ScaledSphere.Radius, 0.f);

    if (DistToCenter > KINDA_SMALL_NUMBER)
    {
        Normal = -Dir.GetUnsafeNormal();
    }
    else
    {
        Normal = FVector::ZeroVector;
    }

    ClosestWorldPosition = WorldPosition - Normal * DistToEdge;

    return DistToEdge;
}

FKBoxElem FKBoxElem::GetFinalScaled(const FVector& Scale3D, const FTransform& RelativeTM) const
{
    float MinScale, MinScaleAbs;
    FVector Scale3DAbs;

    SetupNonUniformHelper(Scale3D * RelativeTM.GetScale3D(), MinScale, MinScaleAbs, Scale3DAbs);

    FKBoxElem ScaledBox = *this;
    ScaledBox.Extent.X *= Scale3DAbs.X;
    ScaledBox.Extent.Y *= Scale3DAbs.Y;
    ScaledBox.Extent.Z *= Scale3DAbs.Z;

    FTransform ScaleTransform(FQuat::Identity, FVector::ZeroVector, Scale3D);
    FTransform BoxTransform = GetTransform() * RelativeTM * ScaleTransform;
    ScaledBox.SetTransform(BoxTransform);

    return ScaledBox;
}

float FKBoxElem::GetShortestDistanceToPoint(const FVector& WorldPosition, const FTransform& BoneToWorldTM) const
{
    const FKBoxElem& ScaledBox = GetFinalScaled(BoneToWorldTM.GetScale3D(), FTransform::Identity);
    const FTransform LocalToWorldTM = GetTransform() * BoneToWorldTM;
    const FVector LocalPosition = LocalToWorldTM.InverseTransformPositionNoScale(WorldPosition);
    const FVector LocalPositionAbs = FVector::GetAbs(LocalPosition);

    const FVector HalfPoint(ScaledBox.Extent.X * 0.5f, ScaledBox.Extent.Y * 0.5f, ScaledBox.Extent.Z * 0.5f);
    const FVector Delta = LocalPositionAbs - HalfPoint;
    const FVector Errors = FVector(FMath::Max<FVector::FReal>(Delta.X, 0), FMath::Max<FVector::FReal>(Delta.Y, 0), FMath::Max<FVector::FReal>(Delta.Z, 0));
    const float Error = Errors.Size();

    return Error > KINDA_SMALL_NUMBER ? Error : 0.f;
}

float FKBoxElem::GetClosestPointAndNormal(const FVector& WorldPosition, const FTransform& BoneToWorldTM, FVector& ClosestWorldPosition, FVector& Normal) const
{
    const FKBoxElem& ScaledBox = GetFinalScaled(BoneToWorldTM.GetScale3D(), FTransform::Identity);
    const FTransform LocalToWorldTM = GetTransform() * BoneToWorldTM;
    const FVector LocalPosition = LocalToWorldTM.InverseTransformPositionNoScale(WorldPosition);

    const float HalfX = ScaledBox.Extent.X * 0.5f;
    const float HalfY = ScaledBox.Extent.Y * 0.5f;
    const float HalfZ = ScaledBox.Extent.Z * 0.5f;

    const FVector ClosestLocalPosition(FMath::Clamp<FVector::FReal>(LocalPosition.X, -HalfX, HalfX), FMath::Clamp<FVector::FReal>(LocalPosition.Y, -HalfY, HalfY), FMath::Clamp<double>(LocalPosition.Z, -HalfZ, HalfZ));
    ClosestWorldPosition = LocalToWorldTM.TransformPositionNoScale(ClosestLocalPosition);

    const FVector LocalDelta = LocalPosition - ClosestLocalPosition;
    float Error = LocalDelta.Size();

    bool bIsOutside = Error > KINDA_SMALL_NUMBER;

    const FVector LocalNormal = bIsOutside ? LocalDelta.GetUnsafeNormal() : FVector::ZeroVector;

    ClosestWorldPosition = LocalToWorldTM.TransformPositionNoScale(ClosestLocalPosition);
    Normal = LocalToWorldTM.TransformVectorNoScale(LocalNormal);

    return bIsOutside ? Error : 0.f;
}

void FKSphylElem::ScaleElem(FVector DeltaSize, float MinSize)
{
    float DeltaRadius = DeltaSize.X;
    if (FMath::Abs(DeltaSize.Y) > FMath::Abs(DeltaRadius))
    {
        DeltaRadius = DeltaSize.Y;
    }

    float DeltaHeight = DeltaSize.Z;
    float radius = FMath::Max(Radius + DeltaRadius, MinSize);
    float length = Length + DeltaHeight;

    length += Radius - radius;
    length = FMath::Max(0.f, length);

    Radius = radius;
    Length = length;
}

FKSphylElem FKSphylElem::GetFinalScaled(const FVector& Scale3D, const FTransform& RelativeTM) const
{
    FKSphylElem ScaledSphylElem = *this;

    float MinScale, MinScaleAbs;
    FVector Scale3DAbs;

    SetupNonUniformHelper(Scale3D * RelativeTM.GetScale3D(), MinScale, MinScaleAbs, Scale3DAbs);

    ScaledSphylElem.Radius = GetScaledRadius(Scale3DAbs);
    ScaledSphylElem.Length = GetScaledCylinderLength(Scale3DAbs);

    const FTransform ScaleTransform(FQuat::Identity, FVector::ZeroVector, Scale3D);
    const FTransform RotationTransform(ScaledSphylElem.Rotation, FVector::ZeroVector, Scale3D);
    const FTransform ScaledRotationTransform = RotationTransform * ScaleTransform;

    const FVector LocalOrigin = RelativeTM.TransformPosition(Center) * Scale3D;
    ScaledSphylElem.Center = LocalOrigin;
    ScaledSphylElem.Rotation = FRotator(RelativeTM.GetRotation() * ScaledRotationTransform.GetRotation());

    return ScaledSphylElem;
}

float FKSphylElem::GetScaledRadius(const FVector& Scale3D) const
{
    const FVector Scale3DAbs = FVector::GetAbs(Scale3D);
    const float RadiusScale = FMath::Max(Scale3DAbs.X, Scale3DAbs.Y);
    return FMath::Clamp(Radius * RadiusScale, 0.1f, GetScaledHalfLength(Scale3DAbs));
}

float FKSphylElem::GetScaledCylinderLength(const FVector& Scale3D) const
{
    return FMath::Max<float>(0.1f, (GetScaledHalfLength(Scale3D) - GetScaledRadius(Scale3D)) * 2.f);
}

float FKSphylElem::GetScaledHalfLength(const FVector& Scale3D) const
{
    return FMath::Max<float>((Length + Radius * 2.0f) * FMath::Abs(Scale3D.Z) * 0.5f, 0.1f);
}

float FKSphylElem::GetShortestDistanceToPoint(const FVector& WorldPosition, const FTransform& BoneToWorldTM) const
{
    const FKSphylElem ScaledSphyl = GetFinalScaled(BoneToWorldTM.GetScale3D(), FTransform::Identity);

    const FTransform LocalToWorldTM = GetTransform() * BoneToWorldTM;
    const FVector ErrorScale = LocalToWorldTM.GetScale3D();
    const FVector LocalPosition = LocalToWorldTM.InverseTransformPositionNoScale(WorldPosition);
    const FVector LocalPositionAbs = FVector::GetAbs(LocalPosition);


    const FVector Target(LocalPositionAbs.X, LocalPositionAbs.Y, FMath::Max<FVector::FReal>(LocalPositionAbs.Z - ScaledSphyl.Length * 0.5f, 0.f));	//If we are above half length find closest point to cap, otherwise to cylinder
    const float Error = FMath::Max(Target.Size() - ScaledSphyl.Radius, 0.f);

    return Error > KINDA_SMALL_NUMBER ? Error : 0.f;
}

float FKSphylElem::GetClosestPointAndNormal(const FVector& WorldPosition, const FTransform& BoneToWorldTM, FVector& ClosestWorldPosition, FVector& Normal) const
{
    const FKSphylElem ScaledSphyl = GetFinalScaled(BoneToWorldTM.GetScale3D(), FTransform::Identity);

    const FTransform LocalToWorldTM = GetTransform() * BoneToWorldTM;
    const FVector ErrorScale = LocalToWorldTM.GetScale3D();
    const FVector LocalPosition = LocalToWorldTM.InverseTransformPositionNoScale(WorldPosition);

    const float HalfLength = 0.5f * ScaledSphyl.Length;
    const float TargetZ = FMath::Clamp<FVector::FReal>(LocalPosition.Z, -HalfLength, HalfLength);	//We want to move to a sphere somewhere along the capsule axis

    const FVector WorldSphere = LocalToWorldTM.TransformPositionNoScale(FVector(0.f, 0.f, TargetZ));
    const FVector Dir = WorldSphere - WorldPosition;
    const float DistToCenter = Dir.Size();
    const float DistToEdge = FMath::Max(DistToCenter - ScaledSphyl.Radius, 0.f);

    bool bIsOutside = DistToCenter > KINDA_SMALL_NUMBER;
    if (bIsOutside)
    {
        Normal = -Dir.GetUnsafeNormal();
    }
    else
    {
        Normal = FVector::ZeroVector;
    }

    ClosestWorldPosition = WorldPosition - Normal * DistToEdge;

    return bIsOutside ? DistToEdge : 0.f;
}

UBodySetup::~UBodySetup()
{
    if (PhysMaterial)
        delete PhysMaterial;
}

void UBodySetup::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    //OutProperties.Add(TEXT("BoxExtent"), AggGeom.ToString());
}

void UBodySetup::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);
    //const FString* TempStr = nullptr;
    //TempStr = InProperties.Find(TEXT("BoxExtent"));
    //if (TempStr)
    //{
        //BoxExtent.InitFromString(*TempStr);
    //}
}

void UBodySetup::DisplayProperty()
{
    Super::DisplayProperty();
    for (const auto& Property : StaticClass()->GetProperties())
    {
        Property->DisplayInImGui(this);
    }
}

void UBodySetup::SetBodyShape(bool Box, bool Sphere, bool Capsule, bool Convex, const UStaticMeshComponent* Comp)
{
    AggGeom.BoxElems.Empty();
    AggGeom.SphereElems.Empty();
    AggGeom.SphylElems.Empty();
    AggGeom.ConvexElems.Empty();
    FStaticMeshRenderData* StaticMeshData = Comp->GetStaticMesh()->GetRenderData();
    FVector Center = (StaticMeshData->BoundingBoxMax + StaticMeshData->BoundingBoxMin) * 0.5f;
    FVector Extent = (StaticMeshData->BoundingBoxMax - StaticMeshData->BoundingBoxMin) * 0.5f;
    float MidExtent;
    float MaxExtent = FMath::Max(FMath::Max(Extent.X, Extent.Y), Extent.Z);
    int Orientation[2];
    if (MaxExtent == Extent.X)
    {
        Orientation[0] = 0;
        MidExtent = FMath::Max(Extent.Y, Extent.Z);
        if (MidExtent == Extent.Y)
            Orientation[1] = 1;
        else
            Orientation[1] = 2;
    }
    else if (MaxExtent == Extent.Y)
    {
        Orientation[0] = 1;
        MidExtent = FMath::Max(Extent.X, Extent.Z);
        if (MidExtent == Extent.X)
            Orientation[1] = 0;
        else
            Orientation[1] = 2;
    }
    else
    {
        Orientation[0] = 2;
        MidExtent = FMath::Max(Extent.X, Extent.Y);
        if (MidExtent == Extent.X)
            Orientation[1] = 0;
        else
            Orientation[1] = 1;
    }
    if (Box)
    {
        FKBoxElem Box(Center, Extent);
        AggGeom.BoxElems.Add(Box);
    }
    if (Sphere)
    {
        FKSphereElem Sphere(Center, MaxExtent);
        AggGeom.SphereElems.Add(Sphere);
    }
    else if (Capsule)
    {
        float Radius = MidExtent;
        FKSphylElem Capsule(Radius, MaxExtent - Radius);
        FRotator Rotation;
        switch (Orientation[0])
        {
        case 0:
            break;
        case 1:
            Rotation = FRotator(FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(90.f)));
            break;
        case 2:
            Rotation = FRotator(FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(90.f)));
            break;
        }
        Capsule.Rotation = Rotation;
        AggGeom.SphylElems.Add(Capsule);
    }
    else if (Convex)
    {
        FKConvexElem Convex;
        Convex.Center = Center;
        TArray<FVector> Vertices;
        TArray<FVector> Normals;
        
        for (const auto& Vertex : StaticMeshData->Vertices)
        {
            FVector Data(Vertex.X, Vertex.Y, Vertex.Z);
            Vertices.Add(Data);
            FVector NormData(Vertex.NormalX, Vertex.NormalY, Vertex.NormalZ);
            Normals.Add(NormData);
        }
        Convex.VertexData = Vertices;
        Convex.FaceIndexBuffer = StaticMeshData->Indices;
        Convex.FaceNormalBuffer = Normals;

        AggGeom.ConvexElems.Add(Convex);
    }
    
}
