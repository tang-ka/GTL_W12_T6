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

// 내용이 별로 없어서 커스템 에러콜백 여기서 구현함
class PhysXErrorCallback : public PxErrorCallback
{
public:
    virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};

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
        const PxQuat& Rotation,
        const TArray<PxShape*> Shapes,
        class UPhysicalMaterial* Material = nullptr);

    void Simulate(float DeltaTime);

    PxPhysics* GetPhysics() { return Physics; }
    PxScene* GetScene() { return Scene; }
    PxCooking* GetCooking() { return Cooking; }

    void RemoveGameObject(GameObject* InGameObject);

private:
    PxDefaultAllocator Allocator;
    PhysXErrorCallback ErrorCallback;
    PxFoundation* Foundation = nullptr;
    PxPhysics* Physics = nullptr;
    PxScene* Scene = nullptr;
    PxCooking* Cooking = nullptr;
    PxDefaultCpuDispatcher* Dispatcher = nullptr;

    TArray<GameObject*> GameObjects;
    TArray<GameObject*> PendingRemoveGameObjects;

    // 콜백 시스템
    FPhysicsSimulationEventCallback* SimCallback = nullptr;
};

