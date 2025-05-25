#include "PhysicsManager.h"
#include "Physics/PhysicsSimulationEventCallback.h"
#include "Userinterface/Console.h"

UPhysicsManager::UPhysicsManager()
{
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
#endif

    // Physics Core
    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), true, pvd);

    PxMaterial* DefaultMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    // Scene Configuration
    PxSceneDesc SceneDesc(Physics->getTolerancesScale());
    SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    Dispatcher = PxDefaultCpuDispatcherCreate(4);

    SceneDesc.cpuDispatcher = Dispatcher;

    SceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
    SceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    SceneDesc.flags |= PxSceneFlag::eENABLE_PCM;

    SceneDesc.filterShader = PxDefaultSimulationFilterShader;

    SimCallback = new FPhysicsSimulationEventCallback();
    SceneDesc.simulationEventCallback = SimCallback;

    Scene = Physics->createScene(SceneDesc);

    Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *Foundation, PxCookingParams(Physics->getTolerancesScale()));
}

GameObject* UPhysicsManager::SpawnGameObject(const PxVec3& Position,
    const PxQuat& Rotation,
    const TArray<PxShape*> Shapes,
    UPhysicalMaterial* Material)
{
    SCOPED_READ_LOCK(*Scene);

    // 엔진에서 생성한 메쉬 형상에 맞는 Body를 생성해주면 될 듯
    //PxMaterial* PxMaterial = Material ? Material->GetPhysXMaterial() : Physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxMaterial* PxMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxTransform transform(Position, Rotation);
    PxRigidDynamic* body = Physics->createRigidDynamic(transform);

    for (PxShape* Shape : Shapes)
    {
        body->attachShape(*Shape);
        Shape->release();
    }

    PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
    Scene->addActor(*body);

    GameObject* NewGameObject = new GameObject(body);
    GameObjects.Add(NewGameObject);

    return NewGameObject;
}

void UPhysicsManager::Simulate(float DeltaTime)
{
    Scene->simulate(DeltaTime);
    Scene->fetchResults(true);

    for (GameObject* Object : GameObjects)
    {
        Object->UpdateFromPhysics();
    }

    for (GameObject* Object : PendingRemoveGameObjects)
    {
        GameObjects.Remove(Object);
    }
}

void UPhysicsManager::RemoveGameObject(GameObject* InGameObject)
{
    PendingRemoveGameObjects.Add(InGameObject);
}

void GameObject::UpdateFromPhysics()
{
    SCOPED_READ_LOCK(*UPhysicsManager::Get().GetScene());
    PxTransform t = rigidBody->getGlobalPose();
    PxMat44 mat(t);
    worldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&mat));
}

void PhysXErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
{
    const char* fileName = strrchr(file, '\\');
    
    if (!fileName) fileName = strrchr(file, '/');
    fileName = fileName ? fileName + 1 : file;

    UE_LOG(ELogLevel::Error, TEXT("[PhysX] %s (%s:%d)"), message, fileName, line);
    printf("[PhysX %s] %s (%s:%d)\n", code, message, fileName, line);
}
