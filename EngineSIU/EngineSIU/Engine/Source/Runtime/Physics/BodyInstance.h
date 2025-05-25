#pragma once

#include "BodyInstanceCore.h"
#include "Engine/PhysicsManager.h"

class UBodySetup;

struct FBodyInstance : public FBodyInstanceCore
{
public:
    FBodyInstance();
    void InitBody(UBodySetup* Setup, const FTransform& WorldTransform);
    void TermBody();

    GameObject* GetActor() const { return Actor; }
    void SetActor(GameObject* InActor) { Actor = InActor; }

    void SetBodyTransform(const FTransform& NewTransform);

private:
    GameObject* Actor = nullptr;
};
