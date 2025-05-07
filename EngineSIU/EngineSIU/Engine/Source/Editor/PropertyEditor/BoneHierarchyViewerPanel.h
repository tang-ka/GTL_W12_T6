#pragma once
#include "GameFramework/Actor.h"
#include "UnrealEd/EditorPanel.h"

class USkeletalMesh;
class FReferenceSkeleton;
class BoneHierarchyViewerPanel : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    void SetSkeletalMesh(USkeletalMesh* SMesh);

private:
    float Width = 600, Height = 100;
    USkeletalMesh* SkeletalMesh;

    void LoadBoneIcon();
    void RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex);
    
    FString GetCleanBoneName(const FString& InFullName);

    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;

    
};
