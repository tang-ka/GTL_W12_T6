#include <algorithm>

#include "Components/StaticMeshComponent.h"

#include "Engine/FObjLoader.h"
#include "Launch/EngineLoop.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"

#include "GameFramework/Actor.h"
#include "Physics/BodyInstance.h"
#include "Engine/PhysicsManager.h"
#include "PhysicsEngine/BodySetup.h"
#include "Physics/PhysicalMaterial.h"

#include "World/World.h"
#include <Actors/Cube.h>
#include "ProjectileMovementComponent.h"

UStaticMeshComponent::UStaticMeshComponent()
{
    Body = new FBodyInstance();
}

UStaticMeshComponent::~UStaticMeshComponent()
{
    if (Body && PhysicsBody)
        Body->TermBody();
}

void UStaticMeshComponent::BeginPlay()
{
    if (StaticMesh->GetBodySetup() && bSimulatePhysics)
    {
        Body->InitBody(this, StaticMesh->GetBodySetup(), GetWorldTransform(), bIsStatic);
    }
}

UObject* UStaticMeshComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));

    NewComponent->bSimulatePhysics = bSimulatePhysics;
    NewComponent->bIsStatic = bIsStatic;
    NewComponent->bSimulateGravity = bSimulateGravity;
    NewComponent->SetStaticMesh(StaticMesh);
    NewComponent->SelectedSubMeshIndex = SelectedSubMeshIndex;
    NewComponent->SetBodySetupGeom(bIsBox, bIsSphere, bIsCapsule, bIsConvex);

    return NewComponent;
}

void UStaticMeshComponent::GetProperties(TMap<FString, FString>& OutProperties) const
{
    Super::GetProperties(OutProperties);
    
    //StaticMesh 경로 저장
    UStaticMesh* CurrentMesh = GetStaticMesh(); 
    if (CurrentMesh != nullptr) {

        // 1. std::wstring 경로 얻기
        std::wstring PathWString = CurrentMesh->GetOjbectName(); // 이 함수가 std::wstring 반환 가정

        // 2. std::wstring을 FString으로 변환
        FString PathFString(PathWString.c_str()); // c_str()로 const wchar_t* 얻어서 FString 생성
       // PathFString = CurrentMesh->ConvertToRelativePathFromAssets(PathFString);

        FWString PathWString2 = PathFString.ToWideString();

        
        OutProperties.Add(TEXT("StaticMeshPath"), PathFString);
    } else
    {
        OutProperties.Add(TEXT("StaticMeshPath"), TEXT("None")); // 메시 없음 명시
    }

    OutProperties.Add(TEXT("bIsBox"), FString::Printf("%d", (static_cast<int>(bIsBox))));
    OutProperties.Add(TEXT("bIsSphere"), FString::Printf("%d", (static_cast<int>(bIsSphere))));
    OutProperties.Add(TEXT("bIsCapsule"), FString::Printf("%d", (static_cast<int>(bIsCapsule))));
    OutProperties.Add(TEXT("bIsConvex"), FString::Printf("%d", (static_cast<int>(bIsConvex))));
}

void UStaticMeshComponent::SetProperties(const TMap<FString, FString>& InProperties)
{
    Super::SetProperties(InProperties);
    const FString* TempStr = nullptr;

    
    // --- StaticMesh 설정 ---
    TempStr = InProperties.Find(TEXT("StaticMeshPath"));
    if (TempStr) // 키가 존재하는지 확인
    {
        if (*TempStr != TEXT("None")) // 값이 "None"이 아닌지 확인
        {
            // 경로 문자열로 UStaticMesh 에셋 로드 시도
           
            if (UStaticMesh* MeshToSet = FObjManager::CreateStaticMesh(*TempStr))
            {
                SetStaticMesh(MeshToSet); // 성공 시 메시 설정
                UE_LOG(ELogLevel::Display, TEXT("Set StaticMesh '%s' for %s"), **TempStr, *GetName());
            }
            else
            {
                // 로드 실패 시 경고 로그
                UE_LOG(ELogLevel::Warning, TEXT("Could not load StaticMesh '%s' for %s"), **TempStr, *GetName());
                SetStaticMesh(nullptr); // 안전하게 nullptr로 설정
            }
        }
        else // 값이 "None"이면
        {
            SetStaticMesh(nullptr); // 명시적으로 메시 없음 설정
            UE_LOG(ELogLevel::Display, TEXT("Set StaticMesh to None for %s"), *GetName());
        }
    }
    else // 키 자체가 없으면
    {
        // 키가 없는 경우 어떻게 처리할지 결정 (기본값 유지? nullptr 설정?)
        // 여기서는 기본값을 유지하거나, 안전하게 nullptr로 설정할 수 있습니다.
        // SetStaticMesh(nullptr); // 또는 아무것도 안 함
        UE_LOG(ELogLevel::Display, TEXT("StaticMeshPath key not found for %s, mesh unchanged."), *GetName());
    }

    const FString* IsBox = InProperties.Find(TEXT("bIsBox"));
    if (IsBox)
    {
        bIsBox = IsBox->ToBool();
    }
    const FString* IsSphere = InProperties.Find(TEXT("bIsSphere"));
    if (IsSphere)
    {
        bIsSphere = IsSphere->ToBool();
    }
    const FString* IsCapsule = InProperties.Find(TEXT("bIsCapsule"));
    if (IsCapsule)
    {
        bIsCapsule = IsCapsule->ToBool();
    }
    const FString* IsConvex = InProperties.Find(TEXT("bIsConvex"));
    if (IsConvex)
    {
        bIsConvex = IsConvex->ToBool();
    }
}

uint32 UStaticMeshComponent::GetNumMaterials() const
{
    if (StaticMesh == nullptr) return 0;

    return StaticMesh->GetMaterials().Num();
}

UMaterial* UStaticMeshComponent::GetMaterial(uint32 ElementIndex) const
{
    if (StaticMesh != nullptr)
    {
        if (OverrideMaterials[ElementIndex] != nullptr)
        {
            return OverrideMaterials[ElementIndex];
        }
    
        if (StaticMesh->GetMaterials().IsValidIndex(ElementIndex))
        {
            return StaticMesh->GetMaterials()[ElementIndex]->Material;
        }
    }
    return nullptr;
}

uint32 UStaticMeshComponent::GetMaterialIndex(FName MaterialSlotName) const
{
    if (StaticMesh == nullptr) return -1;

    return StaticMesh->GetMaterialIndex(MaterialSlotName);
}

TArray<FName> UStaticMeshComponent::GetMaterialSlotNames() const
{
    TArray<FName> MaterialNames;
    if (StaticMesh == nullptr) return MaterialNames;

    for (const FStaticMaterial* Material : StaticMesh->GetMaterials())
    {
        MaterialNames.Emplace(Material->MaterialSlotName);
    }

    return MaterialNames;
}

void UStaticMeshComponent::GetUsedMaterials(TArray<UMaterial*>& Out) const
{
    if (StaticMesh == nullptr)
    {
        return;
    }

    StaticMesh->GetUsedMaterials(Out);
    for (int MaterialIndex = 0; MaterialIndex < GetNumMaterials(); MaterialIndex++)
    {
        if (OverrideMaterials[MaterialIndex] != nullptr)
        {
            Out[MaterialIndex] = OverrideMaterials[MaterialIndex];
        }
    }
}

int UStaticMeshComponent::CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const
{
    if (!AABB.Intersect(InRayOrigin, InRayDirection, OutHitDistance))
    {
        return 0;
    }
    if (StaticMesh == nullptr)
    {
        return 0;
    }
    
    OutHitDistance = FLT_MAX;
    
    int IntersectionNum = 0;

    FStaticMeshRenderData* RenderData = StaticMesh->GetRenderData();

    const TArray<FStaticMeshVertex>& Vertices = RenderData->Vertices;
    const int32 VertexNum = Vertices.Num();
    if (VertexNum == 0)
    {
        return 0;
    }
    
    const TArray<UINT>& Indices = RenderData->Indices;
    const int32 IndexNum = Indices.Num();
    const bool bHasIndices = (IndexNum > 0);
    
    int32 TriangleNum = bHasIndices ? (IndexNum / 3) : (VertexNum / 3);
    for (int32 i = 0; i < TriangleNum; i++)
    {
        int32 Idx0 = i * 3;
        int32 Idx1 = i * 3 + 1;
        int32 Idx2 = i * 3 + 2;
        
        if (bHasIndices)
        {
            Idx0 = Indices[Idx0];
            Idx1 = Indices[Idx1];
            Idx2 = Indices[Idx2];
        }

        // 각 삼각형의 버텍스 위치를 FVector로 불러옵니다.
        FVector V0 = FVector(Vertices[Idx0].X, Vertices[Idx0].Y, Vertices[Idx0].Z);
        FVector V1 = FVector(Vertices[Idx1].X, Vertices[Idx1].Y, Vertices[Idx1].Z);
        FVector V2 = FVector(Vertices[Idx2].X, Vertices[Idx2].Y, Vertices[Idx2].Z);

        float HitDistance = FLT_MAX;
        if (IntersectRayTriangle(InRayOrigin, InRayDirection, V0, V1, V2, HitDistance))
        {
            OutHitDistance = FMath::Min(HitDistance, OutHitDistance);
            IntersectionNum++;
        }

    }
    return IntersectionNum;
}

void UStaticMeshComponent::SetStaticMesh(UStaticMesh* Value)
{
    StaticMesh = Value;
    if (Body)
    {
        Body->TermBody();
    }

    if (StaticMesh == nullptr)
    {
        OverrideMaterials.SetNum(0);
        AABB = FBoundingBox(FVector::ZeroVector, FVector::ZeroVector);
    }
    else
    {
        OverrideMaterials.SetNum(Value->GetMaterials().Num());
        AABB = FBoundingBox(StaticMesh->GetRenderData()->BoundingBoxMin, StaticMesh->GetRenderData()->BoundingBoxMax);
        StaticMesh->GetBodySetup()->SetBodyShape(bIsBox, bIsSphere, bIsCapsule, bIsConvex, this);
    }
}

void UStaticMeshComponent::SimulatePhysics(bool Value)
{
    bSimulatePhysics = Value;
    if (bSimulatePhysics)
    {
        if (StaticMesh && StaticMesh->GetBodySetup())
            Body->InitBody(this, StaticMesh->GetBodySetup(), GetWorldTransform(), bIsStatic);
    }
    else
    {
        Body->TermBody();
    }
}

void UStaticMeshComponent::SetPhysMaterial(float InStaticFric, float InDynamicFric, float InRestitution)
{
    StaticMesh->SetPhysMaterial(InStaticFric, InDynamicFric, InRestitution);
    if (Body)
        Body->TermBody();
    if (bSimulatePhysics)
    {
        if (StaticMesh->GetBodySetup())
            Body->InitBody(this, StaticMesh->GetBodySetup(), GetWorldTransform(), bIsStatic);
    }
}

void UStaticMeshComponent::SimulateGravity(bool Value)
{
    bSimulateGravity = Value;
    if (!PhysicsBody)
        return;
    PhysicsBody->rigidBody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !Value);
    if (Value)
    {
        if (PhysicsBody->rigidBody->is<PxRigidDynamic>())
        {
            PxRigidDynamic* DynamicBody = static_cast<PxRigidDynamic*>(PhysicsBody->rigidBody);
            DynamicBody->wakeUp();
        }
    }
    else
    {
        if (PhysicsBody->rigidBody->is<PxRigidDynamic>())
        {
            PxRigidDynamic* DynamicBody = static_cast<PxRigidDynamic*>(PhysicsBody->rigidBody);
            PxVec3 oldVelocity = DynamicBody->getLinearVelocity();
            oldVelocity.z = 0.f;
            DynamicBody->setLinearVelocity(oldVelocity);
        }
    }
}

void UStaticMeshComponent::CheckPhysSize()
{
    if (!bSimulatePhysics || !PhysicsBody)
        return;
    FVector Extent = (AABB.MaxLocation - AABB.MinLocation) * 0.5f * RelativeScale3D;
    PxShape* Shape;
    PhysicsBody->rigidBody->getShapes(&Shape, 1);
    PxGeometryHolder Geom = Shape->getGeometry();
    if (Geom.getType() == PxGeometryType::eBOX)
    {
        PxBoxGeometry Box = Geom.box();;
        FVector BoxExtent(Box.halfExtents.x, Box.halfExtents.y, Box.halfExtents.z);
        PxTransform LocalPose = Shape->getLocalPose();
        if (BoxExtent != Extent)
        {
            PhysicsBody->rigidBody->detachShape(*Shape);
            PxBoxGeometry BoxGeom(PxVec3(Extent.X, Extent.Y, Extent.Z));
            PxShape* NewShape = UPhysicsManager::Get().GetPhysics()->createShape(BoxGeom, *StaticMesh->GetPhysMaterial()->GetMaterial());
            NewShape->setLocalPose(LocalPose);
            PhysicsBody->rigidBody->attachShape(*NewShape);
            NewShape->release();
        }
    }
}

void UStaticMeshComponent::HandlePhysicsContact(USceneComponent* A, USceneComponent* B)
{
    if (A != this && B != this)
        return;
    USceneComponent* Me = (A == this) ? A : B;
    UE_LOG(ELogLevel::Display, "%s got Hit!", *Me->GetOwner()->GetActorLabel());
}

void UStaticMeshComponent::HandleContactPoint(FVector Pos, FVector Norm)
{
    ACube* ContactEffect = GEngine->ActiveWorld->SpawnActor<ACube>();
    
    ContactEffect->SetActorScale(FVector(0.1f));
    FVector Loc;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 3);

    int rand = dis(gen);
    switch (rand)
    {
    case 0:
        Loc = Pos + FVector(1, 0, 0);
        break;
    case 1:
        Loc = Pos + FVector(0, 1, 0);
        break;
    case 2:
        Loc = Pos + FVector(-1, 0, 0);
        break;
    case 3:
        Loc = Pos + FVector(0, -1, 0);
        break;
    default:
        break;
    }

    ContactEffect->SetActorLocation(Loc);
    UProjectileMovementComponent* Comp = ContactEffect->AddComponent<UProjectileMovementComponent>();
    FVector Front = (Loc - GetComponentLocation()).GetSafeNormal();
    FVector RotAxis = FVector::CrossProduct(FVector(1, 0, 0), Front);
    float Dot = FVector::DotProduct(FVector(1, 0, 0), Front);
    float RotAngle = FMath::Acos(Dot);
    ContactEffect->SetActorRotation(FRotator(FQuat(RotAxis, RotAngle).GetNormalized()));
    Comp->SetInitialSpeed(2.f);
    Comp->SetMaxSpeed(10.0f);
}

void UStaticMeshComponent::SetBodySetupGeom(bool Box, bool Sphere, bool Capsule, bool Convex)
{
    bIsBox = Box; bIsSphere = Sphere; bIsCapsule = Capsule; bIsConvex = Convex;
    EShape NewShape;
    if (Box)
        NewShape = EBox;
    else if (Sphere)
        NewShape = ESphere;
    else if (Capsule)
        NewShape = ECapsule;
    else
        NewShape = EConvex;
    if(NewShape!=CurShape)
        StaticMesh->GetBodySetup()->SetBodyShape(bIsBox, bIsSphere, bIsCapsule, bIsConvex, this);
    CurShape = NewShape;
}

void UStaticMeshComponent::GetBodySetupGeom(bool& OutBox, bool& OutSphere, bool& OutCapsule, bool& OutConvex)
{
    OutBox = bIsBox; OutSphere = bIsSphere; OutCapsule = bIsCapsule; OutConvex = bIsConvex;
}

void UStaticMeshComponent::SetIsStatic(bool Value)
{
    bIsStatic = Value;
    if (Body)
        Body->TermBody();
    if (bSimulatePhysics)
    {
        if (StaticMesh->GetBodySetup())
            Body->InitBody(this, StaticMesh->GetBodySetup(), GetWorldTransform(), bIsStatic);
    }
}
