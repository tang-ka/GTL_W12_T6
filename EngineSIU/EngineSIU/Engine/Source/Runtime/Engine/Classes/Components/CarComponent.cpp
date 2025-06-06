#include "CarComponent.h"
#include "LevelEditor/SlateAppMessageHandler.h"
#include "UObject/ObjectFactory.h"
#include "GameFramework/Actor.h"
#include <Actors/CarActor.h>

UCarComponent::UCarComponent()
{
    
}

UCarComponent::~UCarComponent()
{
    if (bHasBody)
    {
        RemovePhysBody();
        UPhysicsManager::Get().Car = nullptr;
    }
}

UObject* UCarComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));
    
    return NewComponent;
}

void UCarComponent::UpdatePhysics()
{
    if (!bHasBody)
        return;
    SCOPED_READ_LOCK(*UPhysicsManager::Get().GetScene());

    PxTransform BodyT = CarBody->getGlobalPose();
    PxMat44 mat(BodyT);
    XMMATRIX worldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&mat));

    XMFLOAT4X4 temp;
    XMStoreFloat4x4(&temp, worldMatrix); // XMMATRIX → XMFLOAT4X4로 복사

    FMatrix WorldMatrix;
    memcpy(&WorldMatrix, &temp, sizeof(FMatrix)); // 안전하게 float[4][4] 복사

    FVector Location = WorldMatrix.GetTranslationVector();
    FQuat Quat = WorldMatrix.GetMatrixWithoutScale().ToQuat();
    FVector Scale = WorldMatrix.GetScaleVector();

    SetWorldLocation(Location);
    SetWorldRotation(FRotator(Quat));

    for (int i = 0; i < 4; ++i)
    {
        PxTransform WheelT = Wheels[i]->getGlobalPose();
        PxMat44 WheelMat(WheelT);
        worldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&WheelMat));

        XMFLOAT4X4 tempWheel;
        XMStoreFloat4x4(&tempWheel, worldMatrix); // XMMATRIX → XMFLOAT4X4로 복사

        WorldMatrix;
        memcpy(&WorldMatrix, &tempWheel, sizeof(FMatrix)); // 안전하게 float[4][4] 복사

        Location = WorldMatrix.GetTranslationVector();
        Quat = WorldMatrix.GetMatrixWithoutScale().ToQuat();
        Scale = WorldMatrix.GetScaleVector();

        WheelComp[i]->SetWorldLocation(Location);
        WheelComp[i]->SetWorldRotation(FRotator(Quat));
    }
}

void UCarComponent::BeginPlay()
{
    TArray<USceneComponent*> DestroyComps;
    for (auto& Comp : GetAttachChildren())
    {
        bool bIsWheel = false;
        for (int i = 0; i < 4; ++i)
        {
            if (Comp == WheelComp[i])
                bIsWheel = true;
        }
        if (!bIsWheel)
            DestroyComps.Add(Comp);
    }
    for (auto& Comp : DestroyComps)
    {
        Comp->DestroyComponent();
    }
    SpawnComponents();
    AddPhysBody();
    UPhysicsManager::Get().Car = this;
}

void UCarComponent::SpawnComponents()
{
    SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Dodge/Car_RemoveWheel.obj"));
    SetWorldLocation(CarBodyPos);
    SetWorldScale3D(FVector(1.f));
    for (int i = 0; i < 4; ++i)
    {
        AActor* Owner = GetOwner();
        WheelComp[i] = GetOwner()->AddComponent<UStaticMeshComponent>();
        WheelComp[i]->SetupAttachment(GetOwner()->GetRootComponent());
        WheelComp[i]->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Tire/tire.obj"));
        WheelComp[i]->SetWorldLocation(FVector(WheelPos[i].x, WheelPos[i].y, WheelPos[i].z));
        WheelComp[i]->SetWorldScale3D(WheelSize);
        UE_LOG(ELogLevel::Warning, "Spawn Wheel");
    }
}

void UCarComponent::MoveCar()
{
    if (GetKeyState(VK_RBUTTON) & 0x8000 || !bHasBody)
    {
        for (int i = 0; i < 2; ++i)
        {
            RJoints[i]->setDriveVelocity(0.f);
            FJoints[i]->setDriveVelocity(PxVec3(0.f), PxVec3(0.f));
        }
        SteeringJoint->setDriveVelocity(0);
        return;
    }

    if (PressedKeys.Contains(EKeys::A))
    {
        SteeringJoint->setDriveVelocity(DeltaSteerAngle);
        UE_LOG(ELogLevel::Display, "A Pressed!");
    }
    if (PressedKeys.Contains(EKeys::D))
    {
        SteeringJoint->setDriveVelocity(-DeltaSteerAngle);
        UE_LOG(ELogLevel::Display, "D Pressed!");
    }

    if (PressedKeys.Contains(EKeys::W))
    {
        for (int i = 0; i < 2; ++i)
        {
            RJoints[i]->setDriveVelocity(15.f);
            FJoints[i]->setDriveVelocity(PxVec3(0.f), PxVec3(0, 30.f, 0));
        }
        UE_LOG(ELogLevel::Display, "W Pressed!");
    }

    if (PressedKeys.Contains(EKeys::S))
    {
        for (int i = 0; i < 2; ++i)
        {
            RJoints[i]->setDriveVelocity(-15.f);
            FJoints[i]->setDriveVelocity(PxVec3(0.f), PxVec3(0, -30.f, 0));
        }
        UE_LOG(ELogLevel::Display, "S Pressed!");
    }

    if (PressedKeys.IsEmpty())
    {
        for (int i = 0; i < 2; ++i)
        {
            RJoints[i]->setDriveVelocity(0.f);
            FJoints[i]->setDriveVelocity(PxVec3(0.f), PxVec3(0.f));
        }
        SteeringJoint->setDriveVelocity(0);
    }
}

void UCarComponent::AddPhysBody()
{
    PxPhysics* Physics = UPhysicsManager::Get().GetPhysics();
    PxScene* Scene = UPhysicsManager::Get().GetScene();
    if (!DefaultMaterial)
        DefaultMaterial = Physics->createMaterial(2.f, 2.f, 0.f);

    //몸통
    BodyExtent = (AABB.MaxLocation - AABB.MinLocation) * 0.5f;
    BodyExtent.Z *= 0.4f;
    PxBoxGeometry CarBodyGeom(BodyExtent.ToPxVec3());
    CarBody = Physics->createRigidDynamic(PxTransform(CarBodyPos.ToPxVec3()));
    {
        PxShape* BodyShape = Physics->createShape(CarBodyGeom, *DefaultMaterial);
        BodyShape->setSimulationFilterData(PxFilterData(ECollisionChannel::ECC_CarBody, 0xFFFF, 0, 0));
        CarBody->attachShape(*BodyShape);
        BodyShape->release();
        PxReal volume = 8 * BodyExtent.X * BodyExtent.Y * BodyExtent.Z;
        PxReal density = 1200.0f / volume;
        PxRigidBodyExt::updateMassAndInertia(*CarBody, density);
        Scene->addActor(*CarBody);
    }

    //바퀴
    float WheelRadius = 1.2f;
    PxSphereGeometry WheelGeom(WheelRadius);

    for (int i = 0; i < 4; ++i)
    {
        Wheels[i] = Physics->createRigidDynamic(PxTransform(WheelPos[i]));
        PxShape* WheelShape = Physics->createShape(WheelGeom, *DefaultMaterial);
        WheelShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        WheelShape->setSimulationFilterData(PxFilterData(ECollisionChannel::ECC_Wheel, 0xFFFF, 0, 0));
        Wheels[i]->attachShape(*WheelShape);
        WheelShape->release();
        PxRigidBodyExt::updateMassAndInertia(*Wheels[i], 0.1f);
        Scene->addActor(*Wheels[i]);
    }

    PxTransform bodyPose = CarBody->getGlobalPose();   // e.g. (0,0,1), q=I
    //뒷바퀴 몸체 연결
    for (int i = 0; i < 2; ++i)
    {
        // 1) wheelPose 읽어오기
        PxTransform wheelPose = Wheels[i+2]->getGlobalPose(); // e.g. (1,0.9,0.7), q=I

        // 2) 월드에서의 joint 위치/회전 정의
        PxQuat hingeRotYtoX(PxHalfPi, PxVec3(0, 0, 1));       // Z축 +90° → X_local → Y_world
        PxTransform jointGlobalPose(
            wheelPose.p,                                   // 앵커 위치 = 휠 위치
            hingeRotYtoX                                   // 앵커 회전 = 휠 회전에 Z90° 추가
        );

        // 3) 로컬 프레임 계산
        PxTransform localFrameChassis = bodyPose.getInverse() * jointGlobalPose;
        PxTransform localFrameWheel = wheelPose.getInverse() * jointGlobalPose;

        // 4) joint 생성
        RJoints[i] = PxRevoluteJointCreate(
            *Physics,
            CarBody, localFrameChassis,
            Wheels[i+2], localFrameWheel);

        //Joints[i]->setDriveVelocity(10.0f);

        // 힌지 축 고정(회전자유), 다른 축은 잠금
        RJoints[i]->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
        RJoints[i]->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, false);
        RJoints[i]->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
        RJoints[i]->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
    }

    //차 회전 관련 조인트 세팅
    PxTransform FRWheelT = Wheels[0]->getGlobalPose();
    PxTransform FLWheelT = Wheels[1]->getGlobalPose();
    PxQuat JointOrientation(PxHalfPi, PxVec3(0, 1, 0));
    PxVec3 JointPos = (FRWheelT.p + FLWheelT.p) * 0.5f + PxVec3(0,0,0.4f);
    PxTransform JointT(JointPos, JointOrientation);

    PxTransform HubT(JointPos);
    Hub = Physics->createRigidDynamic(HubT);
    PxVec3 HubSize = PxVec3(0.2f, (FRWheelT.p.y - FLWheelT.p.y) * 0.5f, 0.2f);
    PxShape* HubShape = Physics->createShape(PxBoxGeometry(HubSize), *DefaultMaterial);
    HubShape->setSimulationFilterData(PxFilterData(ECollisionChannel::ECC_CarBody, 0xFFFF, 0, 0));
    Hub->attachShape(*HubShape);

    Scene->addActor(*Hub);

    PxTransform BodyLocal = bodyPose.getInverse() * JointT;
    PxTransform HubLocal = HubT.getInverse() * JointT;

    SteeringJoint = PxRevoluteJointCreate(
        *Physics,
        CarBody, BodyLocal,
        Hub, HubLocal
    );
    SteeringJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
    SteeringJoint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
    SteeringJoint->setLimit(PxJointAngularLimitPair(-MaxSteerAngle, +MaxSteerAngle));
    SteeringJoint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
    SteeringJoint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);

    //허브 앞바퀴 연결
    for (int i = 0; i < 2; ++i)
    {
        PxTransform WheelJointT(Wheels[i]->getGlobalPose().p);
        PxTransform LocalWheel = Wheels[i]->getGlobalPose().getInverse() * WheelJointT;
        PxTransform LocalHub = HubT.getInverse() * WheelJointT;

        FJoints[i] = PxD6JointCreate(
            *Physics,
            Hub, LocalHub,
            Wheels[i], LocalWheel
        );
        FJoints[i]->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
        FJoints[i]->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
        FJoints[i]->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);
        FJoints[i]->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLOCKED);
        FJoints[i]->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLOCKED);
        FJoints[i]->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);

        FJoints[i]->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
        PxD6JointDrive driveConfig(
            /*stiffness=*/ 5000.0f,
            /*damping=*/   200.0f,
            /*maxForce=*/  PX_MAX_F32,
            /*isAcceleration=*/ true
        );
        FJoints[i]->setDrive(PxD6Drive::eTWIST, driveConfig);

        // 6) 초기 목표 위치(0회전)로 세팅
        FJoints[i]->setDrivePosition(PxTransform(PxQuat(0, PxVec3(0, 1, 0))));
        FJoints[i]->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
    }
    bHasBody = true;
}

void UCarComponent::RemovePhysBody()
{
    PxScene* Scene = UPhysicsManager::Get().GetScene();
    SteeringJoint->release();
    SteeringJoint = nullptr;

    Scene->removeActor(*CarBody);
    Scene->removeActor(*Hub);
    for (int i = 0; i < 4; ++i)
    {
        if (i < 2)
        {
            RJoints[i]->release();
            FJoints[i]->release();
            RJoints[i] = nullptr;
            FJoints[i] = nullptr;
        }
        Scene->removeActor(*Wheels[i]);
    }
    bHasBody = false;
}
