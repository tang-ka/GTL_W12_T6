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

    TArray<FTransform> RefBonePoseTransforms; // 원본 BindPose에서 복사해온 에디팅을 위한 Transform

    void GetCurrentGlobalBoneMatrices(TArray<FMatrix>& OutBoneMatrices) const;

    void SetAnimationEnabled(bool bEnable);

    void SetAnimation(UAnimSequence* InAnimSequence);

    UAnimSequence* GetAnimation() const { return AnimSequence; }
    
private:
    TArray<FTransform> BonePoseTransforms;
    
    USkeletalMesh* SkeletalMeshAsset = nullptr;

    UAnimSequence* AnimSequence = nullptr;

    float ElapsedTime = 0.f;

    bool bPlayAnimation = false;
};
