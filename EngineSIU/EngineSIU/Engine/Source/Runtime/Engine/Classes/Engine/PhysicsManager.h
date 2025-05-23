#pragma once

#include <PxPhysicsAPI.h>
#include <DirectXMath.h>
#include <vector>
#include <mutex>
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/Container/Array.h"

using namespace physx;
using namespace DirectX;

#define SCOPED_READ_LOCK(scene) PxSceneReadLock scopedReadLock(scene);

struct GameObject {
    PxRigidDynamic* rigidBody = nullptr;
    XMMATRIX worldMatrix = XMMatrixIdentity();

    void UpdateFromPhysics();
};

class FPhysicsSimulationEventCallback;

class UPhysicsManager : public UObject
{
    DECLARE_CLASS(UPhysicsManager, UObject)
public:
    UPhysicsManager();
    ~UPhysicsManager() = default;

    static UPhysicsManager& Get()
    {
        static UPhysicsManager Instance;
        return Instance;
    }

    // Physics lifecycle
    void Initialize();
    //void Shutdown();

    // Spawn Physics scne game object
    class GameObject* SpawnGameObject(const PxVec3& Position,
        const PxGeometry& Geometry,
        class UPhysicalMaterial* Material = nullptr);

    void Simulate(float DeltaTime);

    PxScene* GetScene();

private:
    PxDefaultAllocator Allocator;
    PxDefaultErrorCallback ErrorCallback;
    PxFoundation* Foundation = nullptr;
    PxPhysics* Physics = nullptr;
    PxScene* Scene = nullptr;
    PxDefaultCpuDispatcher* Dispatcher = nullptr;

    TArray<GameObject*> GameObjects;

    // 콜백 시스템
    FPhysicsSimulationEventCallback* SimCallback = nullptr;
};
