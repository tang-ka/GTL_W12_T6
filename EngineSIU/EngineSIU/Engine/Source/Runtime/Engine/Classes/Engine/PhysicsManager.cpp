#include "PhysicsManager.h"
#include "Physics/PhysicsSimulationEventCallback.h"

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
}

GameObject* UPhysicsManager::SpawnGameObject(const PxVec3& Position,
    const PxGeometry& Geometry,
    UPhysicalMaterial* Material)
{
    SCOPED_READ_LOCK(*Scene);

    //PxMaterial* PxMaterial = Material ? Material->GetPhysXMaterial() : Physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxMaterial* PxMaterial = Physics->createMaterial(0.5f, 0.5f, 0.6f);
    PxTransform transform(Position);
    PxRigidDynamic* body = Physics->createRigidDynamic(transform);

    PxShape* shape = Physics->createShape(Geometry, *PxMaterial);
    body->attachShape(*shape);
    shape->release();

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
}

PxScene* UPhysicsManager::GetScene()
{
    return Scene;
}

void GameObject::UpdateFromPhysics()
{
    SCOPED_READ_LOCK(*UPhysicsManager::Get().GetScene());
    PxTransform t = rigidBody->getGlobalPose();
    PxMat44 mat(t);
    worldMatrix = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&mat));
}
