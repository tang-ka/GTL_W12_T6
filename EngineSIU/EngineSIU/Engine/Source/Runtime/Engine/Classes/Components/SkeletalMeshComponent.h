#pragma once
#include "SkinnedMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/Asset/SkeletalMeshAsset.h"

class UAnimSequence;
class USkeletalMesh;

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

    void SetCurrentKey(int InCurrentKey) { CurrentKey = InCurrentKey; }

    void SetElapsedTime(float InSeconds) { ElapsedTime = InSeconds; }
    
private:
    TArray<FTransform> BonePoseTransforms;
    
    USkeletalMesh* SkeletalMeshAsset = nullptr;

    UAnimSequence* AnimSequence = nullptr;

    float ElapsedTime = 0.f;
    float TargetKeyFrame = 0.f;
    float Alpha = 0.f;
    int32 CurrentKey = 0.f;
    bool bPlayAnimation = false;

    std::unique_ptr<FSkeletalMeshRenderData> CPURenderData;

    static bool bCPUSkinning;
};
