#pragma once
#include "Actors/Player.h"
#include "Misc/Optional.h"
#include "Container/String.h"

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

    void test()
    {
        TOptional<FString> OptStr("hello");
        TOptional<FString> OpStr2 = FString("hello");
    }

    UPROPERTY(
        EditAnywhere, ({ .DisplayName = FString("My u8"), .ToolTip = FString("wa u8") }),
        uint8, u8, = 0;
    )

    UPROPERTY(
        EditAnywhere, { .ToolTip = FString("wa 123") },
        uint8, gudtldn, = 0;
    )

    UPROPERTY(
        EditAnywhere, ({ .ToolTip = {"hide alpha"}, .HideAlphaChannel = true }),
        FColor, color, = FColor::Blue;
    )

    UPROPERTY(
        EditAnywhere, ({ .ToolTip = {"hide alpha linear"}, .HideAlphaChannel = true }),
        FLinearColor, lcolor, = FLinearColor::Black;
    )

    UPROPERTY(
        EditAnywhere, ({ .ToolTip = {"DragDeltaSpeed 123"}, .DragDeltaSpeed = 123, .ClampMin = 20.0f, .ClampMax = 200.0f }),
        uint32, u32, = 0;
    )

    UPROPERTY(
        EditAnywhere, ({ .ToolTip = {"DragDeltaSpeed 0.123f"}, .DragDeltaSpeed = 0.123f }),
        float, f32, = 0;
    )

    UPROPERTY(
        EditAnywhere, ({ .ToolTip = {"DragDeltaSpeed 0.123f"}, .InlineEditConditionToggle = true }),
        bool, my_bool, = false;
    )

    UPROPERTY(
        EditAnywhere, { .ToolTip = {"Inline String"} },
        FString, InlineString, ;
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
