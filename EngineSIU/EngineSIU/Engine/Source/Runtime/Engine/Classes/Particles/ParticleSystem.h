#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UParticleEmitter;

class UFXSystemAsset : public UObject
{
    DECLARE_CLASS(UFXSystemAsset, UObject)

public:
    UFXSystemAsset() = default;
    virtual ~UFXSystemAsset() override = default;
};


class UParticleSystem : public UFXSystemAsset
{
    DECLARE_CLASS(UParticleSystem, UObject)

public:
    UParticleSystem() = default;
    virtual ~UParticleSystem() override = default;

    TArray<UParticleEmitter*> GetEmitters() const;

private:
    TArray<UParticleEmitter*> Emitters;
};
