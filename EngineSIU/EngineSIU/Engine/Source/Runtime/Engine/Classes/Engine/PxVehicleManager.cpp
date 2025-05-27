#include "PxVehicleManager.h"
#include "Engine.h"
#include "World/World.h"
#include "StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/FObjLoader.h"

void UPxVehicleManager::Initialize(PxPhysics* InPhysics, PxScene* InScene)
{
    Physics = InPhysics;
    Scene = InScene;
    DefaultMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    //몸통
    PxBoxGeometry CarBodyGeom(PxVec3(1.5f, 0.5f, 0.3f));
    CarBody = Physics->createRigidDynamic(PxTransform(PxVec3(0.f, 0.f, 1.f)));
    {
        PxShape* BodyShape = Physics->createShape(CarBodyGeom, *DefaultMaterial);
        BodyShape->setSimulationFilterData(PxFilterData(ECollisionChannel::ECC_CarBody, 0xFFFF, 0, 0));
        CarBody->attachShape(*BodyShape);
        BodyShape->release();
        PxReal volume = (2 * 1.5f) * (2 * 0.5f) * (2 * 0.3f);
        PxReal density = 1200.0f / volume;
        PxRigidBodyExt::updateMassAndInertia(*CarBody, density);
        Scene->addActor(*CarBody);
    }

    //바퀴
    PxSphereGeometry WheelGeom(0.35f);

    for (int i = 0; i < 4; ++i)
    {
        Wheels[i] = Physics->createRigidDynamic(PxTransform(WheelPos[i]));
        PxShape* WheelShape = Physics->createShape(WheelGeom, *DefaultMaterial);
        WheelShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        WheelShape->setSimulationFilterData(PxFilterData(ECollisionChannel::ECC_Wheel, 0xFFFF, 0, 0));
        Wheels[i]->attachShape(*WheelShape);
        WheelShape->release();
        PxRigidBodyExt::updateMassAndInertia(*Wheels[i], 20.0f);
        Scene->addActor(*Wheels[i]);
    }

    for (int i = 0; i < 4; ++i)
    {
        // 1) bodyPose / wheelPose 읽어오기
        PxTransform bodyPose = CarBody->getGlobalPose();   // e.g. (0,0,1), q=I
        PxTransform wheelPose = Wheels[i]->getGlobalPose(); // e.g. (1,0.9,0.7), q=I
    
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
        PxRevoluteJoint* joint = PxRevoluteJointCreate(
            *Physics,
            CarBody, localFrameChassis,
            Wheels[i], localFrameWheel);
    
        joint->setDriveVelocity(10.0f);
    
        // 힌지 축 고정(회전자유), 다른 축은 잠금
        joint->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
        joint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, false);
        joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
    }
    AddActors();
}

void UPxVehicleManager::Tick(float DeltaTime)
{
    if (!bHasActor)
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

    BodyActor->SetActorLocation(Location);
    BodyActor->SetActorRotation(FRotator(Quat));

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

        WheelActors[i]->SetActorLocation(Location);
        WheelActors[i]->SetActorRotation(FRotator(Quat));
    }
}

void UPxVehicleManager::AddActors()
{
    BodyActor = GEngine->ActiveWorld->SpawnActor<AStaticMeshActor>();
    Cast<AStaticMeshActor>(BodyActor)->GetStaticMeshComponent()->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Primitives/CubePrimitive.obj"));
    BodyActor->SetActorLocation(FVector(0, 0, 1));
    BodyActor->SetActorScale(FVector(3, 1, 0.6f));
    for (int i = 0; i < 4; ++i)
    {
        WheelActors[i] = GEngine->ActiveWorld->SpawnActor<AStaticMeshActor>();
        Cast<AStaticMeshActor>(WheelActors[i])->GetStaticMeshComponent()->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/Reference/Reference.obj"));
        WheelActors[i]->SetActorLocation(FVector(WheelPos[i].x, WheelPos[i].y, WheelPos[i].z));
        WheelActors[i]->SetActorScale(FVector(0.35f));
    }
    bHasActor = true;
}
