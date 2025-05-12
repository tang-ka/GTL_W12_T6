#pragma once
#include "SkinnedMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/Asset/SkeletalMeshAsset.h"
#include "Template/SubclassOf.h"

class UAnimSequence;
class USkeletalMesh;
class UAnimInstance;
class UAnimSingleNodeInstance;

enum class EAnimationMode : uint8
{
    AnimationBlueprint,
    AnimationSingleNode,
};

class USkeletalMeshComponent : public USkinnedMeshComponent
{
    DECLARE_CLASS(USkeletalMeshComponent, USkinnedMeshComponent)

public:
    USkeletalMeshComponent();
    virtual ~USkeletalMeshComponent() override;

    virtual void InitializeComponent() override;

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void TickComponent(float DeltaTime) override;

    virtual void TickPose(float DeltaTime) override;

    bool ShouldTickAnimation() const;

    bool InitializeAnimScriptInstance();

    void ClearAnimScriptInstance();

    USkeletalMesh* GetSkeletalMeshAsset() const { return SkeletalMeshAsset; }

    void SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset);

    TArray<FTransform> RefBonePoseTransforms; // 원본 BindPose에서 복사해온 에디팅을 위한 Transform

    void GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const;

    void DEBUG_SetAnimationEnabled(bool bEnable);

    void PlayAnimation(UAnimationAsset* NewAnimToPlay, bool bLooping);

    void SetAnimation(UAnimationAsset* NewAnimToPlay);

    UAnimationAsset* GetAnimation() const;

    void Play(bool bLooping);

    void Stop();

    bool IsPlaying() const;

    void SetPlayRate(float Rate);

    float GetPlayRate() const;

    void SetLooping(bool bIsLooping);

    bool IsLooping() const;
    
    bool bIsAnimationEnabled() const { return bPlayAnimation; }
    
    virtual int CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const override;

    const FSkeletalMeshRenderData* GetCPURenderData() const;

    static void SetCPUSkinning(bool flag);

    static bool GetCPUSkinning();
    
    float GetElapsedTime() const{ return ElapsedTime; }
    float GetTargetKeyFrame() const { return TargetKeyFrame; }
    int GetCurrentKey() const { return CurrentKey; }

    void SetCurrentKey(int InCurrentKey) { CurrentKey = InCurrentKey; }

    void SetElapsedTime(float InSeconds) { ElapsedTime = InSeconds; }

    UAnimInstance* GetAnimInstance() const { return AnimScriptInstance; }

    void SetAnimationMode(EAnimationMode InAnimationMode);

    EAnimationMode GetAnimationMode() const { return AnimationMode; }

    virtual void InitAnim();
    
protected:
    bool NeedToSpawnAnimScriptInstance() const;

    EAnimationMode AnimationMode;
    
private:
    TArray<FTransform> BonePoseTransforms;
    
    USkeletalMesh* SkeletalMeshAsset;

    UAnimSequence* AnimSequence;

    float ElapsedTime = 0.f;
    
    float TargetKeyFrame = 0.f;
    
    float Alpha = 0.f;
    
    int32 CurrentKey = 0.f;
    
    bool bPlayAnimation;

    std::unique_ptr<FSkeletalMeshRenderData> CPURenderData;

    static bool bCPUSkinning;

public:
    TSubclassOf<UAnimInstance> AnimClass;
    
    UAnimInstance* AnimScriptInstance;

    UAnimSingleNodeInstance* GetSingleNodeInstance() const;
};
