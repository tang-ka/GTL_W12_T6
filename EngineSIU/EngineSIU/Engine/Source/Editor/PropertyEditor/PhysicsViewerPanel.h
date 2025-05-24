#pragma once
#include "Engine/EditorEngine.h"
#include "GameFramework/Actor.h"
#include "UnrealEd/EditorPanel.h"

class USkeletalMesh;
class FReferenceSkeleton;
class USkeletalMeshComponent;
class UAnimDataModel;
class PhysicsViewerPanel : public UEditorPanel
{
public:
    PhysicsViewerPanel();

    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

    void SetSkeletalMesh(USkeletalMesh* SMesh);

    int32 GetSelectedBoneIndex() const;
    FString GetSelectedBoneName() const;

    void ClearRefSkeletalMeshComponent();
private:
    float Width = 0, Height = 0;
    USkeletalMesh* SkeletalMesh;

    void LoadBoneIcon();
    void CopyRefSkeleton();

    void RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex, UEditorEngine* Engine);
    // Body - Constraint 연결 정보 표시
    void RenderConstraintPanel();
    void RenderPhysicsDetailPanel();

    FString GetCleanBoneName(const FString& InFullName);

    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;

    int32 SelectedBoneIndex = INDEX_NONE;

    FReferenceSkeleton* CopiedRefSkeleton = nullptr;
    USkeletalMeshComponent* RefSkeletalMeshComponent = nullptr;
};
