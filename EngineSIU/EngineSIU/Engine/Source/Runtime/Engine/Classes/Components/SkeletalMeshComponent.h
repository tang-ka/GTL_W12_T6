#pragma once
#include "SkinnedMeshComponent.h"
#include "Engine/AssetManager.h"

class UAnimSequence;
class USkeletalMesh;

class USkeletalMeshComponent : public USkinnedMeshComponent
{
    DECLARE_CLASS(USkeletalMeshComponent, USkinnedMeshComponent)

public:
    USkeletalMeshComponent();
    virtual ~USkeletalMeshComponent() override;

    void TickComponent(float DeltaTime) override;

    USkeletalMesh* GetSkeletalMeshAsset() const { return SkeletalMeshAsset; }

    void SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset);

    UAnimSequence* AnimSequence = nullptr;

    TArray<FTransform> BoneTransforms;

    TArray<FTransform> BoneBindPoseTransforms; // 원본 BindPose에서 복사해온 에디팅을 위한 Transform

    void GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const;

    void SetAnimationEnabled(bool bEnable);
    bool bIsAnimationEnabled() const { return bPlayAnimation; }
    
    virtual int CheckRayIntersection(const FVector& InRayOrigin, const FVector& InRayDirection, float& OutHitDistance) const override;

    float GetElapsedTime() const{ return ElapsedTime; }
    float GetTargetKeyFrame() const { return TargetKeyFrame; }
    int GetCurrentKey() const { return CurrentKey; }

    void SetCurrentKey(int InCurrentKey) { CurrentKey = InCurrentKey; }

    void SetElapsedTime(float InSeconds) { ElapsedTime = InSeconds; }

    

private:
    USkeletalMesh* SkeletalMeshAsset = nullptr;
    float ElapsedTime = 0.f;
    float TargetKeyFrame = 0.f;
    float Alpha = 0.f;
    int32 CurrentKey = 0.f;
    bool bPlayAnimation = false;
};
