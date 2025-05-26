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
private:
    TMap<int32, USceneComponent*> BoneComponentMap;

    float Width = 0, Height = 0;
    USkeletalMesh* SkeletalMesh;

    void LoadBoneIcon();
    void CopyRefSkeleton();

    void RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex, UEditorEngine* Engine);
    // Body - Constraint 연결 정보 표시
    void RenderConstraintPanel();
    void RenderPhysicsDetailPanel();

    void RenderBodySetupEditor(UBodySetup* BodySetup);

    UBodySetup* FindBodySetupForBone(int32 BoneIndex);

    FString GetCleanBoneName(const FString& InFullName);

    void GenerateBoxBodiesForAllBones();

    FTransform CalculateBoneWorldTransform(int32 BoneIndex);

    //void ClearExistingBoxComponents();

    void CreatePhysicsBodySetup(const FMeshBoneInfo& BoneInfo, const FTransform& BoneTransform, int32 BoneIndex);

    void CreateBoxComponentForBone(int32 BoneIndex, const FTransform& BoneTransform, const FName& BoneName);

    void CreateHierarchicalBoxComponent(int32 BoneIndex, const FTransform& BoneLocalTransform, const FName& BoneName);

    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;

    int32 SelectedBoneIndex = INDEX_NONE;

    FReferenceSkeleton* CopiedRefSkeleton = nullptr;
    USkeletalMeshComponent* RefSkeletalMeshComponent = nullptr;
};
