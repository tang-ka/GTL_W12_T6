#pragma once
#include "SkinnedMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/Asset/SkeletalMeshAsset.h"

class UAnimSequence;
class USkeletalMesh;
class FAnimNotifyEvent;
class UAnimSequenceBase;
class USkeletalMeshComponent : public USkinnedMeshComponent
{
    DECLARE_CLASS(USkeletalMeshComponent, USkinnedMeshComponent)

public:
    USkeletalMeshComponent();
    virtual ~USkeletalMeshComponent() override;

    virtual UObject* Duplicate(UObject* InOuter) override;

    virtual void TickComponent(float DeltaTime) override;

    USkeletalMesh* GetSkeletalMeshAsset() const { return SkeletalMeshAsset; }

    void SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset);

    TArray<FTransform> RefBonePoseTransforms; // 원본 BindPose에서 복사해온 에디팅을 위한 Transform

    void GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const;

    void SetAnimationEnabled(bool bEnable);

    void SetAnimation(UAnimSequence* InAnimSequence);

    UAnimSequence* GetAnimation() const { return AnimSequence; }
    bool bIsAnimationEnabled() const { return bPlayAnimation; }
    
    virtual int CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const override;

    const FSkeletalMeshRenderData* GetCPURenderData() const;

    static void SetCPUSkinning(bool flag);

    static bool GetCPUSkinning();
    
    float GetElapsedTime() const{ return ElapsedTime; }
    float GetTargetKeyFrame() const { return TargetKeyFrame; }
    int GetCurrentKey() const { return CurrentKey; }
    float GetAlpha() const { return Alpha; }
    
    void SetCurrentKey(int InCurrentKey) { CurrentKey = InCurrentKey; }
    void SetElapsedTime(float InSeconds) { ElapsedTime = InSeconds; }
    
    // PlaySpeed
    float GetPlaySpeed() const;
    void SetPlaySpeed(float InSpeed);

    // Loop Frame
    int32 GetLoopStartFrame() const;
    void SetLoopStartFrame(int32 InStart);
    int32 GetLoopEndFrame() const;
    void SetLoopEndFrame(int32 InEnd);

    // Reverse
    bool IsPlayReverse() const;
    void SetPlayReverse(bool bEnable);

    // Pause
    bool IsPaused() const;
    void SetPaused(bool bPause);

    // Looping
    bool IsLooping() const;
    void SetLooping(bool bEnable);

    void EvaluateAnimNotifies(const TArray<FAnimNotifyEvent>& Notifies, float CurrentTime, float PreviousTime, float DeltaTime, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* AnimAsset, bool bIsLooping);
    
private:
    TArray<FTransform> BonePoseTransforms;
    
    USkeletalMesh* SkeletalMeshAsset = nullptr;

    UAnimSequence* AnimSequence = nullptr;

    float ElapsedTime = 0.f;
    float PreviousTime = 0.f;
    float TargetKeyFrame = 0.f;
    float Alpha = 0.f;
    int32 CurrentKey = 0.f;
    float PlaySpeed = 1.f;
    
    int32 LoopStartFrame = 0;
    int32 LoopEndFrame = 0;
    
    bool bPlayAnimation = false;
    bool bPauseAnimation = false;
    bool bPlayReverse = false;
    bool bPlayLooping = false;
    

    std::unique_ptr<FSkeletalMeshRenderData> CPURenderData;

    static bool bCPUSkinning;
};
