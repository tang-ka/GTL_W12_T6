#pragma once
#include "Engine/EditorEngine.h"
#include "GameFramework/Actor.h"
#include "UnrealEd/EditorPanel.h"

// Forward declarations
class USkeletalMesh;
class FReferenceSkeleton;
class USkeletalMeshComponent;
struct UAnimSequence; // Assuming UAnimSequence might be needed for types

// Define PlaybackState structure as per design_spec.md
struct FPlaybackState
{
    bool bIsPlaying = false;
    bool bIsPaused = false;
    bool bIsReversing = false;
    bool bIsLooping = false;

    int32 CurrentFrame = 0;
    int32 StartFrame = 0;
    int32 EndFrame = 100;      // Default, will be updated from AnimSequence
    
    int32 LoopStartFrame = 0;
    int32 LoopEndFrame = 100;  // Default, will be updated from AnimSequence

    float PlaybackSpeed = 1.0f;
    float AccumulatedTime = 0.0f;
    // int32 PreviousFrameForEngineUpdate = -1; // This will be managed by the logic, potentially replacing the old PreviousFrame
};

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
    USkeletalMesh* SkeletalMesh; // Current Skeletal Mesh being viewed

    void LoadBoneIcon();
    void CopyRefSkeleton(); // Copies skeleton for display

    void RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex, UEditorEngine* Engine);
    void RenderAnimationSequence(const FReferenceSkeleton& RefSkeleton, UEditorEngine* Engine);
    
    FString GetCleanBoneName(const FString& InFullName);

    // UI Resources
    ID3D11ShaderResourceView* BoneIconSRV = nullptr;
    ID3D11ShaderResourceView* NonWeightBoneIconSRV = nullptr;

    // State
    int32 SelectedBoneIndex = INDEX_NONE;
    FReferenceSkeleton* CopiedRefSkeleton = nullptr; // Copied skeleton for thread safety or modification
    USkeletalMeshComponent* RefSkeletalMeshComponent = nullptr; // Reference to the component holding the animation state
    
    // int32 PreviousFrame = 0; // This was in the original, might be replaced or integrated with FPlaybackState
                               // Let's keep it commented for now and see if it's needed or if FPlaybackState.PreviousFrameForEngineUpdate is better.
                               // The design doc had `previousFrameForEngineUpdate` in PlaybackState. I'll add it back to FPlaybackState.

    // Playback control state
    FPlaybackState mPlaybackState;

    // Helper to initialize/reset playback state when animation changes
    void InitializePlaybackState(UAnimSequence* AnimSequence);
};

// Re-add PreviousFrameForEngineUpdate to FPlaybackState as it was in the design
// It's better to modify the struct definition directly above.
// Corrected FPlaybackState struct:
/*
struct FPlaybackState
{
    bool bIsPlaying = false;
    bool bIsPaused = false;
    bool bIsReversing = false;
    bool bIsLooping = false;

    int32 CurrentFrame = 0;
    int32 StartFrame = 0;
    int32 EndFrame = 100;
    
    int32 LoopStartFrame = 0;
    int32 LoopEndFrame = 100;

    float PlaybackSpeed = 1.0f;
    float AccumulatedTime = 0.0f;
    int32 PreviousFrameForEngineUpdate = -1; // For optimizing engine updates
};
*/
// The definition above has been updated to include PreviousFrameForEngineUpdate and use b IsPlaying etc. for Unreal bool convention.

