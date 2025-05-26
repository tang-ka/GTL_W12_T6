#pragma once 

#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Physics/BodySetupEnums.h"

namespace physx
{
    class PxTriangleMesh;
}

class UBodySetupCore : public UObject
{
    DECLARE_CLASS(UBodySetupCore, UObject)
public:
    UBodySetupCore();
    ~UBodySetupCore() = default;

    virtual void GetProperties(TMap<FString, FString>& OutProperties) const;
    virtual void SetProperties(const TMap<FString, FString>& InProperties);

    ECollisionTraceFlag GetCollisionTraceFlag() const;

    //UPROPERTY(Category = BodySetup, VisibleAnywhere)
    FName BoneName;

    /**
     *	If simulated it will use physics, if kinematic it will not be affected by physics, but can interact with physically simulated bodies. Default will inherit from OwnerComponent's behavior.
     */
    //UPROPERTY(EditAnywhere, Category = Physics)
    EPhysicsType PhysicsType;

    /** Collision Trace behavior - by default, it will keep simple(convex)/complex(per-poly) separate **/
    //UPROPERTY(EditAnywhere, Category = Collision, meta = (DisplayName = "Collision Complexity"))
    ECollisionTraceFlag CollisionTraceFlag;

    /** Collision Type for this body. This eventually changes response to collision to others **/
    //UPROPERTY(EditAnywhere, Category = Collision)
    EBodyCollisionResponse::Type CollisionReponse;
};
