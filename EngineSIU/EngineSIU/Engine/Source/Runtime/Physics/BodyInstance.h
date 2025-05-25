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

private:
    GameObject* Actor = nullptr;
};
