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
    float Width = 300, Height = 100;
    USkeletalMesh* SkeletalMesh;

    void RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex);
};
