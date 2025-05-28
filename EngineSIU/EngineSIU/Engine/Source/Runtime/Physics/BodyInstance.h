#pragma once

#include "BodyInstanceCore.h"
#include "Engine/PhysicsManager.h"

class UBodySetup;
class USceneComponent;

struct FBodyInstance : public FBodyInstanceCore
{
public:
    FBodyInstance();
    void InitBody(USceneComponent* InOwner, UBodySetup* Setup, const FTransform& WorldTransform, const bool bIsStatic = false);
    void TermBody();

    GameObject* GetActor() const { return Actor; }
    void SetActor(GameObject* InActor) { Actor = InActor; }

    FName GetBoneName() const { return BoneName; }
    void SetBoneName(FName InName) { BoneName = InName; }

    void SetBodyTransform(const FTransform& NewTransform);

private:
    GameObject* Actor = nullptr;
    FName BoneName;

public:
    USceneComponent* Owner = nullptr;
};
