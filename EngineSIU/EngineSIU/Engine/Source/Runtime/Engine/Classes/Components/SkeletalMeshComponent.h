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
    void TickComponent(float DeltaTime) override;

    USkeletalMesh* GetSkeletalMesh() const { return SkeletalMesh; }
    void SetSkeletalMesh(USkeletalMesh* InSkeletalMesh);

    UAnimSequence* AnimSequence = nullptr;

    TArray<FTransform> BoneTransforms;
    
private:
    
    float ElapsedTime = 0.f;
    USkeletalMesh* SkeletalMesh = nullptr;
};
