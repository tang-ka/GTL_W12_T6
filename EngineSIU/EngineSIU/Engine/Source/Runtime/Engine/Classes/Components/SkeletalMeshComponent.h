#pragma once
#include "SkinnedMeshComponent.h"

class UAnimSequence;
class USkeletalMesh;

class USkeletalMeshComponent : public USkinnedMeshComponent
{
    DECLARE_CLASS(USkeletalMeshComponent, USkinnedMeshComponent)

public:
    USkeletalMeshComponent();
    virtual ~USkeletalMeshComponent() override;

    virtual void TickComponent(float DeltaTime) override;

    USkeletalMesh* GetSkeletalMeshAsset() const { return SkeletalMeshAsset; }

    void SetSkeletalMeshAsset(USkeletalMesh* InSkeletalMeshAsset);

    UAnimSequence* AnimSequence = nullptr;

    TArray<FTransform> BoneTransforms;
    
private:
    USkeletalMesh* SkeletalMeshAsset = nullptr;

    float ElapsedTime = 0.f;
};
