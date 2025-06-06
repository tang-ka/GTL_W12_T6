#include "PhysicsManager.h"
#include "Physics/PhysicsSimulationEventCallback.h"
#include "Userinterface/Console.h"
#include "Components/SceneComponent.h"
#include <Components/StaticMeshComponent.h>
#include <UObject/Casts.h>
#include "World/World.h"
#include "Components/CarComponent.h"
#include "BodyInstance.h"

UPhysicsManager::UPhysicsManager()
{
    TolerancesScale = new PxTolerancesScale();
}

void UPhysicsManager::Initialize()
{
    // Foundation Initialize
    Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, Allocator, ErrorCallback);

    // PhysX Debugger
#ifdef _DEBUG
    PxPvd* pvd = PxCreatePvd(*Foundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    // Physics Core
    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, *TolerancesScale, true, pvd);
    PxInitExtensions(*Physics, pvd);
#else
    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, *TolerancesScale, true);
#endif


    PxMaterial* DefaultMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    // Scene Configuration
    PxSceneDesc SceneDesc(Physics->getTolerancesScale());
    SceneDesc.gravity = PxVec3(0.0f, 0.0f, -9.81f);
    Dispatcher = PxDefaultCpuDispatcherCreate(4);

    SceneDesc.cpuDispatcher = Dispatcher;

    SceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    SceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    SceneDesc.flags |= PxSceneFlag::eENABLE_PCM;

    SceneDesc.filterShader = MySimulationFilterShader;

    SimCallback = new FPhysicsSimulationEventCallback();
    SceneDesc.simulationEventCallback = SimCallback;

    Scene = Physics->createScene(SceneDesc);

    Scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
    Scene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

#ifdef _DEBUG
    PxPvdSceneClient* PvdClient = Scene->getScenePvdClient();
    if (PvdClient)
    {
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        PvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
#endif

    Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, PxCookingParams(Physics->getTolerancesScale()));
}

GameObject* UPhysicsManager::SpawnGameObject(
    FBodyInstance* InBodyInstance,
    const PxVec3& Position,
    const PxQuat& Rotation,
    const TArray<PxShape*> Shapes,
    const bool bIsStatic,
    const bool bIsKinematic,
    UPhysicalMaterial* Material)
{
    SCOPED_READ_LOCK(*Scene);

    // 엔진에서 생성한 메쉬 형상에 맞는 Body를 생성해주면 될 듯
    //PxMaterial* PxMaterial = Material ? Material->GetPhysXMaterial() : Physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxMaterial* PxMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxTransform transform(Position, Rotation);
    PxRigidActor* body;

    if (bIsStatic)
    {
        PxRigidStatic* StaticBody = Physics->createRigidStatic(transform);
        body = StaticBody;
        body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    }
    else
    {
        PxRigidDynamic* DynamicBody = Physics->createRigidDynamic(transform);
        if (bIsKinematic)
        {
            DynamicBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        }
        else
        {
            PxRigidBodyExt::updateMassAndInertia(*DynamicBody, 10.0f);
        }
        body = DynamicBody;
    }

    for (PxShape* Shape : Shapes)
    {
        body->attachShape(*Shape);
        Shape->release();
    }

    GameObject* NewGameObject = new GameObject(body);
    body->userData = InBodyInstance;
    PendingSpawnGameObjects.Add(NewGameObject);
    //Scene->addActor(*body);
    //GameObjects.Add(NewGameObject);

    return NewGameObject;
}

void UPhysicsManager::Simulate(float DeltaTime)
{
    for (GameObject* Object : GameObjects)
    {
        FBodyInstance* UserDataInstance = reinterpret_cast<FBodyInstance*>(Object->rigidBody->userData);
        USceneComponent* Owner = UserDataInstance->Owner;
        //USceneComponent* Owner = reinterpret_cast<USceneComponent*>(Object->rigidBody->userData);
        if (!Owner)
            continue;

        if (UStaticMeshComponent* StaticComp = Cast<UStaticMeshComponent>(Owner))
        {
            PxTransform CompTransform = Owner->GetComponentTransform().ToPxTransform();
            Object->rigidBody->setGlobalPose(CompTransform);
        }
        //else if (USkeletalMeshComponent* SkeletalComp = Cast<USkeletalMeshComponent>(Owner))
        //{
        //    SkeletalComp->SyncComponentToBody();
        //}
    }

    if (Car)
        Car->MoveCar();

    Scene->simulate(DeltaTime);
    Scene->fetchResults(true);

    if (PendingSpawnGameObjects.Num() > 0)
        int i = 0;
    for (GameObject* Object : PendingSpawnGameObjects)
    {
        SCOPED_WRITE_LOCK(*Scene);
        Scene->addActor(*Object->rigidBody);
        GameObjects.Add(Object);
    }
    PendingSpawnGameObjects.Empty();

    for (GameObject* Object : GameObjects)
    {
        Object->UpdateFromPhysics();
    }

    for (GameObject* Object : GameObjects)
    {
        FBodyInstance* UserDataInstance = reinterpret_cast<FBodyInstance*>(Object->rigidBody->userData);
        USceneComponent* Owner = UserDataInstance->Owner;
        //USceneComponent* Owner = reinterpret_cast<USceneComponent*>(Object->rigidBody->userData);
        
        if (!Owner)
        {
            continue;
        }

        if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Owner))
        {
            // Skeletal Mesh 컴포넌트의 경우, 위치와 회전만 업데이트
            SkeletalMeshComp->SyncBodyToComponent(UserDataInstance->GetBoneName(), FTransform(Object->worldMatrix));
        }
    }

    if (Car)
        Car->UpdatePhysics();
}

void UPhysicsManager::RemoveGameObjects()
{
    SCOPED_WRITE_LOCK(*Scene);
    for (GameObject* Object : PendingRemoveGameObjects)
    {
        Scene->removeActor(*Object->rigidBody);
        GameObjects.Remove(Object);
        Object->Release();
        delete Object;
        Object = nullptr;
    }
    PendingRemoveGameObjects.Empty();
}

void UPhysicsManager::RemoveGameObject(GameObject* InGameObject)
{
    PendingRemoveGameObjects.Add(InGameObject);
}


void UPhysicsManager::InputKey(const FKeyEvent& InKeyEvent)
{
    if (!Car)
        return;
    if (GEngine->ActiveWorld->WorldType == EWorldType::Editor)
        return;
    switch (InKeyEvent.GetCharacter())
    {
    case 'A':
    {
        if (InKeyEvent.GetInputEvent() == IE_Pressed)
        {
            Car->PressedKeys.Add(EKeys::A);
        }
        else if (InKeyEvent.GetInputEvent() == IE_Released)
        {
            Car->PressedKeys.Remove(EKeys::A);
        }
        break;
    }
    case 'D':
    {
        if (InKeyEvent.GetInputEvent() == IE_Pressed)
        {
            Car->PressedKeys.Add(EKeys::D);
        }
        else if (InKeyEvent.GetInputEvent() == IE_Released)
        {
            Car->PressedKeys.Remove(EKeys::D);
        }
        break;
    }
    case 'W':
    {
        if (InKeyEvent.GetInputEvent() == IE_Pressed)
        {
            Car->PressedKeys.Add(EKeys::W);
        }
        else if (InKeyEvent.GetInputEvent() == IE_Released)
        {
            Car->PressedKeys.Remove(EKeys::W);
        }
        break;
    }
    case 'S':
    {
        if (InKeyEvent.GetInputEvent() == IE_Pressed)
        {
            Car->PressedKeys.Add(EKeys::S);
        }
        else if (InKeyEvent.GetInputEvent() == IE_Released)
        {
            Car->PressedKeys.Remove(EKeys::S);
        }
        break;
    }
    }
}

void GameObject::UpdateFromPhysics()
{
    FBodyInstance* UserDataInstance = reinterpret_cast<FBodyInstance*>(rigidBody->userData);
    USceneComponent* Owner = UserDataInstance->Owner;
    //USceneComponent* Owner = reinterpret_cast<USceneComponent*>(rigidBody->userData);
    //if(!Owner || Owner->GetWorld()->WorldType != EWorldType::PIE && Owner->GetWorld()->WorldType != EWorldType::Game)
    //    return;
    SCOPED_READ_LOCK(*UPhysicsManager::Get().GetScene());

    PxTransform t = rigidBody->getGlobalPose();
    PxMat44 mat(t);
    XMMATRIX worldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&mat));

    XMFLOAT4X4 temp;
    XMStoreFloat4x4(&temp, worldMatrix); // XMMATRIX → XMFLOAT4X4로 복사
    this->worldMatrix = worldMatrix;

    FMatrix WorldMatrix;
    memcpy(&WorldMatrix, &temp, sizeof(FMatrix)); // 안전하게 float[4][4] 복사
    
    FVector Location = WorldMatrix.GetTranslationVector();
    FQuat Quat = WorldMatrix.GetMatrixWithoutScale().ToQuat();
    FVector Scale = WorldMatrix.GetScaleVector();

    if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Owner))
    {
        Owner->SetWorldLocation(Location);
        Owner->SetWorldRotation(Quat);
    }
    //else if (USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(Owner))
    //{
    //    // Skeletal Mesh 컴포넌트의 경우, 위치와 회전만 업데이트

    //    SkeletalMeshComp->SyncBodyToComponent(UserDataInstance->GetBoneName(), FTransform(this->worldMatrix));
    //}
}

void GameObject::Release()
{
    rigidBody->release();
    rigidBody = nullptr;
}

void PhysXErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{
    const char* fileName = strrchr(file, '\\');
    
    if (!fileName) fileName = strrchr(file, '/');
    fileName = fileName ? fileName + 1 : file;

    UE_LOG(ELogLevel::Error, TEXT("[PhysX] %s (%s:%d)"), message, fileName, line);
    printf("[PhysX %s] %s (%s:%d)\n", code, message, fileName, line);
}

PxFilterFlags MySimulationFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // 1) 기본 시뮬레이션 필터링
    PxFilterFlags flags = PxDefaultSimulationFilterShader(
        attributes0, filterData0,
        attributes1, filterData1,
        pairFlags, constantBlock, constantBlockSize);

    // 2) 충돌 이벤트를 꼭 받아오도록 플래그 추가
    pairFlags = PxPairFlag::eCONTACT_DEFAULT
        | PxPairFlag::eNOTIFY_TOUCH_FOUND
        | PxPairFlag::eNOTIFY_TOUCH_LOST
        | PxPairFlag::eNOTIFY_CONTACT_POINTS
        | PxPairFlag::eNOTIFY_TOUCH_PERSISTS
        ;

    bool bIsCarBodyWheel = ((filterData0.word0 == ECC_CarBody && filterData1.word0 == ECC_Wheel) ||
        (filterData0.word0 == ECC_Wheel && filterData1.word0 == ECC_CarBody));

    if (bIsCarBodyWheel)
        return PxFilterFlag::eSUPPRESS;

    return flags;
}
