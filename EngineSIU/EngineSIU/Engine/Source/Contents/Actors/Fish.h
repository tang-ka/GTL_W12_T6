#pragma once
#include "Actors/Player.h"

class UFishBodyComponent;
class USphereComponent;
class UStaticMeshComponent;
class UFishTailComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFishHealthChanged, int32 /* CurrentHealth */, int32 /* MaxHealth */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnFishDied, bool /* bZeroHealth */);

enum class ETestEnum : uint8
{
    Test1,
    Test2,
    Test3
};


struct FTestStruct
{
    DECLARE_STRUCT(FTestStruct)

    UPROPERTY(
        EditAnywhere,
        float, TestValue, = 0.0f;
    )

    UPROPERTY(
        EditAnywhere,
        ETestEnum, TestEnum, = ETestEnum::Test1;
    )
};

struct FChildStruct : public FTestStruct
{
    DECLARE_STRUCT(FChildStruct, FTestStruct)

    UPROPERTY(
        EditAnywhere,
        bool, bIsTrue, = true;
    )
    
    UPROPERTY(
        EditAnywhere,
        TArray<FTestStruct>, TestStruct, {};
    )
};

class AFish : public APlayer
{
    DECLARE_CLASS(AFish, APlayer)

public:
    AFish();
    virtual ~AFish() override = default;

    virtual void PostSpawnInitialize() override;

    UObject* Duplicate(UObject* InOuter) override;

    void BeginPlay() override;

    void Tick(float DeltaTime) override;

    int32 GetHealth() const { return Health; }
    void SetHealth(int32 InHealth, bool bShouldNotify = true);

    int32 GetMaxHealth() const { return MaxHealth; }
    void SetMaxHealth(int32 InMaxHealth);

    float GetHealthPercent() const { return static_cast<float>(Health) / static_cast<float>(MaxHealth); }

    bool IsDead() const { return Health <= 0; }
    
    FOnFishHealthChanged OnHealthChanged;

    FOnFishDied OnDied;

    void Reset();

    int32 GetScore() const { return Score; }
    void SetScore(int32 InScore) { Score = InScore; }

    void SetVelocity(FVector InVelocity){Velocity = InVelocity;}
    
protected:
    UPROPERTY
    (EditAnywhere, USphereComponent*, SphereComponent, = nullptr)

    UPROPERTY
    (EditAnywhere, UFishBodyComponent*, FishBody, = nullptr)

    UPROPERTY
    (EditAnywhere, UFishTailComponent*, FishTail, = nullptr)

    UPROPERTY(
        EditAnywhere,
        FVector, Velocity, = FVector::ZeroVector;
    )

    UPROPERTY(
    EditAnywhere, float, JumpZVelocity, = 0;
    )

    float Gravity;

    UPROPERTY_WITH_FLAGS(EditAnywhere,
    bool, bShouldApplyGravity);

    UPROPERTY(
        EditAnywhere,
        FTestStruct, Struct1, {};
    )
    
    UPROPERTY(
        EditAnywhere,
        FChildStruct, Struct2, {}
    )
    
    void Move(float DeltaTime);

    void RotateMesh();

    float MeshPitchMax;

    float MeshPitch;
    
    float MeshRotSpeed = 10.f;

    int32 MaxHealth;

    int32 Health;

    float KillZ;

    int32 Score;

    void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
