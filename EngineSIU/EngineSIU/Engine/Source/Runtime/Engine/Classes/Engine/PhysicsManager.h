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

class USceneComponent;
class UPxVehicleManager;

#define SCOPED_READ_LOCK(scene) PxSceneReadLock scopedReadLock(scene);
#define SCOPED_WRITE_LOCK(scene) PxSceneWriteLock scopedWriteLock(scene);

struct GameObject {
    PxRigidActor* rigidBody = nullptr;
    XMMATRIX worldMatrix = XMMatrixIdentity();

    void UpdateFromPhysics();

    void Release();
};

class FPhysicsSimulationEventCallback;

// 내용이 별로 없어서 커스템 에러콜백 여기서 구현함
class PhysXErrorCallback : public PxErrorCallback
{
public:
    virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPhysicsContact, USceneComponent*, USceneComponent*);

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
    class GameObject* SpawnGameObject(
        USceneComponent* InOwner,
        const PxVec3& Position,
        const PxQuat& Rotation,
        const TArray<PxShape*> Shapes,
        const bool bIsStatic = false,
        const bool bIsKinematic = false,
        class UPhysicalMaterial* Material = nullptr);

    void Simulate(float DeltaTime);

    void RemoveGameObjects();

    int GetRemoveGameObjectNum() { return PendingRemoveGameObjects.Num(); }

    PxPhysics* GetPhysics() { return Physics; }
    PxScene* GetScene() { return Scene; }
    PxCooking* GetCooking() { return Cooking; }
    const PxTolerancesScale* GetTolerancesScale() const { return TolerancesScale; }

    void RemoveGameObject(GameObject* InGameObject);

    void SpawnVehicle();

private:
    PxDefaultAllocator Allocator;
    PhysXErrorCallback ErrorCallback;
    PxFoundation* Foundation = nullptr;
    PxPhysics* Physics = nullptr;
    PxScene* Scene = nullptr;
    PxCooking* Cooking = nullptr;
    PxTolerancesScale* TolerancesScale = nullptr;
    PxDefaultCpuDispatcher* Dispatcher = nullptr;

    TArray<GameObject*> GameObjects;
    TArray<GameObject*> PendingRemoveGameObjects;
    TArray<GameObject*> PendingSpawnGameObjects;

    // 콜백 시스템
    FPhysicsSimulationEventCallback* SimCallback = nullptr;
    

public:
    FOnPhysicsContact OnPhysicsContact;
    UPxVehicleManager* Vehicle = nullptr;
};

enum ECollisionChannel
{
    ECC_CarBody = (1 << 0),
    ECC_Wheel = (1 << 1),
    // 필요 시 다른 채널…
};

PxFilterFlags MySimulationFilterShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

