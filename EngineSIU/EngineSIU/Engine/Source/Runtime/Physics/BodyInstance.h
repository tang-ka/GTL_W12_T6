#pragma once

#include "BodyInstanceCore.h"
#include "Engine/PhysicsManager.h"

class UBodySetup;
class USceneComponent;

struct FBodyInstance : public FBodyInstanceCore
{
public:
    FBodyInstance();
    void InitBody(USceneComponent* InOwner, UBodySetup* Setup, const FTransform& WorldTransform);
    void TermBody();

private:
    GameObject* Actor = nullptr;
    USceneComponent* Owner = nullptr;
};
