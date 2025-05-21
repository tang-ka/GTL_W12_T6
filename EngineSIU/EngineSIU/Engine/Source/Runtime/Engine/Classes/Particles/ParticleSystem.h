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

    TArray<UParticleEmitter*> GetEmitters() const { return Emitters; }
    void AddEmitter(UParticleEmitter* Emitter) { Emitters.Add(Emitter); }
    void DeleteEmitter(UParticleEmitter* Emitter) { Emitters.Remove(Emitter); }

    float GetMacroUVRadius() const { return MacroUVRadius; }
    FVector GetMacroUVPosition() const { return MacroUVPosition; }

private:
    TArray<UParticleEmitter*> Emitters;

    UPROPERTY_WITH_FLAGS(EditAnywhere, float, MacroUVRadius)
    UPROPERTY_WITH_FLAGS(EditAnywhere, FVector, MacroUVPosition)
};
