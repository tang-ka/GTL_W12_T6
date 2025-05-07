#pragma once
#include "GameFramework/Actor.h"
#include "UnrealEd/EditorPanel.h"

class USkeletalMesh;
class FReferenceSkeleton;
class BoneHierarchyViewerPanel : public UEditorPanel
{
public:
    BoneHierarchyViewerPanel();

    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    void SetSkeletalMesh(USkeletalMesh* SMesh);

    int32 GetSelectedBoneIndex() const;
    FString GetSelectedBoneName() const;

private:
    float Width = 0, Height = 0;
    USkeletalMesh* SkeletalMesh;

    void LoadBoneIcon();
    void CopyRefSkeleton();

    void RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex);
    
    FString GetCleanBoneName(const FString& InFullName);

    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;

    int32 SelectedBoneIndex = INDEX_NONE;

    FReferenceSkeleton* CopiedRefSkeleton = nullptr;

};
