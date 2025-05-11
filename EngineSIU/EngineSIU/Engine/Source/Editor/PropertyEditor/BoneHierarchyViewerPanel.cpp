#include "BoneHierarchyViewerPanel.h"
#include "Engine/EditorEngine.h"
#include <ReferenceSkeleton.h> // Should be Engine/Public/ReferenceSkeleton.h or similar if it's an engine type

#include "Animation/AnimData/AnimDataModel.h" // Make sure this path is correct
#include "Engine/Classes/Engine/SkeletalMesh.h"
#include "Engine/Classes/Animation/Skeleton.h"
// #include "Engine/Classes/Engine/FbxLoader.h" // Not directly used in timeline logic
#include "ThirdParty/ImGui/include/ImGui/imgui.h" // Added for ImGui functions
#include "ThirdParty/ImGui/include/ImGui/imgui_neo_sequencer.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Engine/Classes/Animation/AnimSequence.h"

// Helper function to initialize or reset playback state
void BoneHierarchyViewerPanel::InitializePlaybackState(UAnimSequence* AnimSequence)
{
    if (AnimSequence && AnimSequence->GetDataModel())
    {
        const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
        mPlaybackState.StartFrame = 0;
        mPlaybackState.EndFrame = DataModel->GetNumberOfFrames() <= 0 ? 1:DataModel->GetNumberOfFrames() - 1;
        if (mPlaybackState.EndFrame < 0) mPlaybackState.EndFrame = 0; // Handle empty animation

        mPlaybackState.CurrentFrame = mPlaybackState.StartFrame;
        mPlaybackState.LoopStartFrame = mPlaybackState.StartFrame;
        mPlaybackState.LoopEndFrame = mPlaybackState.EndFrame;
        
        mPlaybackState.bIsPlaying = false;
        mPlaybackState.bIsPaused = false;
        mPlaybackState.bIsReversing = false;
        mPlaybackState.PlaybackSpeed = 1.0f;
        mPlaybackState.AccumulatedTime = 0.0f;
        // mPlaybackState.PreviousFrameForEngineUpdate = -1; // Part of struct, default init is fine
    }
    else
    {
        // Default values if no valid animation sequence
        mPlaybackState.StartFrame = 0;
        mPlaybackState.EndFrame = 100;
        mPlaybackState.CurrentFrame = 0;
        mPlaybackState.LoopStartFrame = 0;
        mPlaybackState.LoopEndFrame = 100;
        mPlaybackState.bIsPlaying = false;
        mPlaybackState.bIsPaused = false;
        mPlaybackState.bIsReversing = false;
        mPlaybackState.PlaybackSpeed = 1.0f;
        mPlaybackState.AccumulatedTime = 0.0f;
    }
}

BoneHierarchyViewerPanel::BoneHierarchyViewerPanel()
{
    SetSupportedWorldTypes(EWorldTypeBitFlag::SkeletalViewer);
    InitializePlaybackState(nullptr); 
}

void BoneHierarchyViewerPanel::Render()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return;
    }

    if (BoneIconSRV == nullptr || NonWeightBoneIconSRV == nullptr) {
        LoadBoneIcon();
    }

    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.3f;
    float PanelPosX = (Width) * 0.8f + 5.0f;
    float PanelPosY = 5.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 500);

    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;
    
    if (Engine->ActiveWorld) {
        if (Engine->ActiveWorld->WorldType == EWorldType::SkeletalViewer) {
            if (CopiedRefSkeleton == nullptr) {
                CopyRefSkeleton(); // RefSkeletalMeshComponent가 null이어도 내부에서 처리
                if (RefSkeletalMeshComponent && RefSkeletalMeshComponent->GetAnimation()) {
                    InitializePlaybackState(RefSkeletalMeshComponent->GetAnimation());
                }
            }


            if (CopiedRefSkeleton == nullptr || CopiedRefSkeleton->RawRefBoneInfo.IsEmpty()) {
                ImGui::Begin("Bone Hierarchy", nullptr, PanelFlags);
                ImGui::Text("No skeleton selected or skeleton has no bones.");
                ImGui::End();
            } else {
                ImGui::Begin("Bone Hierarchy", nullptr, PanelFlags);
                for (int32 i = 0; i < CopiedRefSkeleton->RawRefBoneInfo.Num(); ++i)
                {
                    if (CopiedRefSkeleton->RawRefBoneInfo[i].ParentIndex == INDEX_NONE)
                    {
                        RenderBoneTree(*CopiedRefSkeleton, i, Engine);
                    }
                }
                ImGui::End();
            }
        }
        
        if (RefSkeletalMeshComponent) { 
            RenderAnimationSequence(*CopiedRefSkeleton, Engine);
        }
        
        float ExitPanelWidth = (Width) * 0.2f - 6.0f;
        float ExitPanelHeight = 30.0f;
        float ExitPanelPosX = Width - ExitPanelWidth;
        float ExitPanelPosY = Height - ExitPanelHeight - 10;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(ExitPanelWidth, ExitPanelHeight), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(ExitPanelPosX, ExitPanelPosY), ImGuiCond_Always);
        
        constexpr ImGuiWindowFlags ExitPanelFlags =
            ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoBackground
            | ImGuiWindowFlags_NoScrollbar;

        ImGui::Begin("Exit Viewer", nullptr, ExitPanelFlags);
        if (ImGui::Button("Exit Viewer", ImVec2(ExitPanelWidth, ExitPanelHeight))) {
            UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine);
            EdEngine->EndSkeletalMeshViewer();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void BoneHierarchyViewerPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}

void BoneHierarchyViewerPanel::SetSkeletalMesh(USkeletalMesh* SMesh)
{
    SkeletalMesh = SMesh;
    if (RefSkeletalMeshComponent) {
         InitializePlaybackState(RefSkeletalMeshComponent->GetAnimation());
    } else {
         InitializePlaybackState(nullptr);
    }
}

int32 BoneHierarchyViewerPanel::GetSelectedBoneIndex() const
{
    return SelectedBoneIndex;
}

FString BoneHierarchyViewerPanel::GetSelectedBoneName() const
{
    if (SelectedBoneIndex == INDEX_NONE || !SkeletalMesh)
        return TEXT("");
    const auto& RefSkel = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
    return RefSkel.RawRefBoneInfo[SelectedBoneIndex].Name.ToString();
}

void BoneHierarchyViewerPanel::LoadBoneIcon()
{
    BoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Bone_16x.PNG")->TextureSRV;
    NonWeightBoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/BoneNonWeighted_16x.PNG")->TextureSRV;
}

void BoneHierarchyViewerPanel::CopyRefSkeleton()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine || !Engine->SkeletalMeshViewerWorld) return;

    USkeletalMeshComponent* SkelComp = Engine->SkeletalMeshViewerWorld->GetSkeletalMeshComponent();
    if (!SkelComp || !SkelComp->GetSkeletalMeshAsset() || !SkelComp->GetSkeletalMeshAsset()->GetSkeleton())
        return;

    const FReferenceSkeleton& OrigRef = SkelComp->GetSkeletalMeshAsset()->GetSkeleton()->GetReferenceSkeleton();

    if (CopiedRefSkeleton) delete CopiedRefSkeleton;
    CopiedRefSkeleton = new FReferenceSkeleton();
    CopiedRefSkeleton->RawRefBoneInfo = OrigRef.RawRefBoneInfo;
    CopiedRefSkeleton->RawRefBonePose = OrigRef.RawRefBonePose;
    CopiedRefSkeleton->RawNameToIndexMap = OrigRef.RawNameToIndexMap;

    RefSkeletalMeshComponent = SkelComp; // 항상 갱신
}


void BoneHierarchyViewerPanel::RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex, UEditorEngine* Engine)
{
    const FMeshBoneInfo& BoneInfo = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex];
    const FString& ShortBoneName = GetCleanBoneName(BoneInfo.Name.ToString());

    ImGui::PushID(BoneIndex);
    ImGui::Image((ImTextureID)BoneIconSRV, ImVec2(16, 16));
    ImGui::SameLine();

    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
    bool bHasChildren = false;
    for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
    {
        if (RefSkeleton.RawRefBoneInfo[i].ParentIndex == BoneIndex)
        {
            bHasChildren = true;
            break;
        }
    }
    if (!bHasChildren)
    {
        NodeFlags |= ImGuiTreeNodeFlags_Leaf;
        NodeFlags &= ~ImGuiTreeNodeFlags_OpenOnArrow;
    }

    bool bNodeOpen = ImGui::TreeNodeEx(*ShortBoneName, NodeFlags);
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        SelectedBoneIndex = BoneIndex; 
    }

    if (bNodeOpen)
    {
        for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
        {
            if (RefSkeleton.RawRefBoneInfo[i].ParentIndex == BoneIndex)
            {
                RenderBoneTree(RefSkeleton, i, Engine);
            }
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void BoneHierarchyViewerPanel::RenderAnimationSequence(const FReferenceSkeleton& RefSkeleton, UEditorEngine* Engine)
{
    if (!RefSkeletalMeshComponent || !RefSkeletalMeshComponent->GetAnimation())
<<<<<<< HEAD
        return;

    UAnimSequence* AnimSeq = RefSkeletalMeshComponent->GetAnimation();
    UAnimDataModel* DataModel = AnimSeq->GetDataModel();
    if (PrevAnimDataModel != DataModel)
    {
        RefSkeletalMeshComponent->SetLoopStartFrame(0);
        RefSkeletalMeshComponent->SetLoopEndFrame(FMath::Max(1, DataModel->GetNumberOfFrames() - 1));
        PrevAnimDataModel = DataModel;
    }
    
    const int32 FrameRate = DataModel->GetFrameRate();
    const int32 NumFrames = DataModel->GetNumberOfFrames();
    static bool transformOpen = false;
    
    // 게터/세터를 통한 접근으로 변경
    int32 LoopStart = RefSkeletalMeshComponent->GetLoopStartFrame();
    int32 LoopEnd = RefSkeletalMeshComponent->GetLoopEndFrame();
    // 방어 코드 추가
    LoopStart = FMath::Clamp(LoopStart, 0, NumFrames - 2);
    LoopEnd = FMath::Clamp(LoopEnd, LoopStart + 1, NumFrames - 1);

    // 유효성 확인: Start가 End보다 크거나 같거나 음수면 기본 값으로 설정
    if (LoopStart >= LoopEnd || LoopStart < 0 || LoopEnd < 0)
    {
        LoopStart = 0;
        LoopEnd = FMath::Max(1, NumFrames - 1);
        RefSkeletalMeshComponent->SetLoopStartFrame(LoopStart);
        RefSkeletalMeshComponent->SetLoopEndFrame(LoopEnd);
    }

    float PlaySpeed = RefSkeletalMeshComponent->GetPlaySpeed();
    bool bLooping = RefSkeletalMeshComponent->IsLooping();
    bool bReverse = RefSkeletalMeshComponent->IsPlayReverse();
    bool bPaused = RefSkeletalMeshComponent->IsPaused();

    float Elapsed = RefSkeletalMeshComponent->GetElapsedTime();
    float TargetKeyFrame = Elapsed * FrameRate;
    int32 CurrentFrame = static_cast<int32>(TargetKeyFrame) % (LoopEnd + 1);
    PreviousFrame = CurrentFrame;

    ImGui::SetNextWindowSize(ImVec2(400, 220), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Animation Sequence Timeline"))
    {
        if (ImGui::Button("Play")) {
            RefSkeletalMeshComponent->SetAnimationEnabled(true);
            RefSkeletalMeshComponent->SetPaused(false);
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            RefSkeletalMeshComponent->SetPaused(true);
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            RefSkeletalMeshComponent->SetAnimationEnabled(false);
        }

        ImGui::Separator();

        if (ImGui::SliderFloat("Play Speed", &PlaySpeed, 0.1f, 3.0f, "%.1fx"))
            RefSkeletalMeshComponent->SetPlaySpeed(PlaySpeed);

        if (ImGui::Checkbox("Looping", &bLooping))
            RefSkeletalMeshComponent->SetLooping(bLooping);

        if (ImGui::Checkbox("Play Reverse", &bReverse))
            RefSkeletalMeshComponent->SetPlayReverse(bReverse);

        if (ImGui::SliderInt("Loop Start", &LoopStart, 0, NumFrames - 2))
            RefSkeletalMeshComponent->SetLoopStartFrame(LoopStart);

        if (ImGui::SliderInt("Loop End", &LoopEnd, LoopStart + 1, NumFrames - 1))
            RefSkeletalMeshComponent->SetLoopEndFrame(LoopEnd);

        // 슬라이더 조정 후 유효성 재검사
        LoopStart = FMath::Clamp(LoopStart, 0, NumFrames - 2);
        LoopEnd = FMath::Clamp(LoopEnd, LoopStart + 1, NumFrames - 1);

        if (LoopStart >= LoopEnd || LoopStart < 0 || LoopEnd < 0)
        {
            LoopStart = 0;
            LoopEnd = FMath::Max(1, NumFrames - 1);
            RefSkeletalMeshComponent->SetLoopStartFrame(LoopStart);
            RefSkeletalMeshComponent->SetLoopEndFrame(LoopEnd);
        }

        

        if (ImGui::BeginNeoSequencer("Sequencer", &CurrentFrame, &LoopStart, &LoopEnd)) {
            if (ImGui::BeginNeoGroup("Transform", &transformOpen)) {
                std::vector<ImGui::FrameIndexType> keys = {0, 10, 24};
                if (ImGui::BeginNeoTimeline("Position", keys)) {
                    ImGui::EndNeoTimeLine();
=======
    {
        ImGui::Begin("Animation Timeline", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Text("No Animation Loaded or Skeletal Mesh Component not set.");
        ImGui::End();
        return;
    }

    UAnimSequence* AnimSequence = RefSkeletalMeshComponent->GetAnimation();
    const UAnimDataModel* DataModel = AnimSequence->GetDataModel();
    if (!DataModel)
    {
        ImGui::Begin("Animation Timeline", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Text("Animation Data Model is not available.");
        ImGui::End();
        return;
    }

    if (mPlaybackState.EndFrame != (DataModel->GetNumberOfFrames() -1) && DataModel->GetNumberOfFrames() > 0) {
        InitializePlaybackState(AnimSequence);
    }
    mPlaybackState.CurrentFrame = RefSkeletalMeshComponent->GetElapsedTime() * DataModel->GetFrameRate();
    
    const float FrameRate = DataModel->GetFrameRate();
    int32 previousCurrentFrameForEngine = mPlaybackState.CurrentFrame; // Store current frame before any UI or logic changes it this tick
    
    ImGui::SetNextWindowSize(ImVec2(Width * 0.78f, 220), ImGuiCond_FirstUseEver); 
    ImGui::SetNextWindowPos(ImVec2(10, Height - 230), ImGuiCond_FirstUseEver); 

    if (ImGui::Begin("Animation Timeline", nullptr, ImGuiWindowFlags_HorizontalScrollbar))
    {
        int32 frameBeforeSequencer = mPlaybackState.CurrentFrame;

        if (mPlaybackState.bIsPlaying && !mPlaybackState.bIsPaused && mPlaybackState.PlaybackSpeed != 0.0f && FrameRate > 0.0f)
        {
            mPlaybackState.AccumulatedTime += ImGui::GetIO().DeltaTime;
            float timePerFrame = 1.0f / FrameRate;
            float effectiveTimePerFrame = timePerFrame / fabsf(mPlaybackState.PlaybackSpeed);

            if (effectiveTimePerFrame > 0) 
            {
                int framesToAdvance = static_cast<int>(mPlaybackState.AccumulatedTime / effectiveTimePerFrame);
                if (framesToAdvance > 0)
                {
                    mPlaybackState.AccumulatedTime -= framesToAdvance * effectiveTimePerFrame;
                    if (mPlaybackState.bIsReversing || mPlaybackState.PlaybackSpeed < 0.0f) 
                    {
                        mPlaybackState.CurrentFrame -= framesToAdvance;
                    }
                    else
                    {
                        mPlaybackState.CurrentFrame += framesToAdvance;
                    }
>>>>>>> cac7cb82 (UI 임시 작업중)
                }
            }
        }

        if (ImGui::BeginNeoSequencer("Sequencer", &mPlaybackState.CurrentFrame, &mPlaybackState.StartFrame, &mPlaybackState.EndFrame, ImVec2(0,60) /*ImGuiNeoSequencerFlags_HideZoom*/))
        {
            ImGui::EndNeoSequencer();
        }
<<<<<<< HEAD
=======
        // If sequencer changed the frame, reset accumulated time
        if (mPlaybackState.CurrentFrame != frameBeforeSequencer) {
            mPlaybackState.AccumulatedTime = 0.0f;
        }

        ImGui::Spacing();

        if (mPlaybackState.bIsPlaying && !mPlaybackState.bIsPaused)
        {
            if (ImGui::Button("Pause [❚❚]")) { mPlaybackState.bIsPaused = true; }
        }
        else
        {
            if (ImGui::Button("Play  [▶]")) 
            {
                mPlaybackState.bIsPlaying = true;
                mPlaybackState.bIsPaused = false;
                if (!mPlaybackState.bIsReversing && mPlaybackState.CurrentFrame >= mPlaybackState.EndFrame && mPlaybackState.EndFrame > mPlaybackState.StartFrame) { // Check EndFrame > StartFrame to avoid issues with single frame anims
                    mPlaybackState.CurrentFrame = mPlaybackState.bIsLooping ? mPlaybackState.LoopStartFrame : mPlaybackState.StartFrame;
                }
                else if (mPlaybackState.bIsReversing && mPlaybackState.CurrentFrame <= mPlaybackState.StartFrame && mPlaybackState.EndFrame > mPlaybackState.StartFrame) {
                     mPlaybackState.CurrentFrame = mPlaybackState.bIsLooping ? mPlaybackState.LoopEndFrame : mPlaybackState.EndFrame;
                }
                mPlaybackState.AccumulatedTime = 0.0f; 
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Stop [■]"))
        {
            mPlaybackState.bIsPlaying = false;
            mPlaybackState.bIsPaused = false;
            mPlaybackState.CurrentFrame = mPlaybackState.StartFrame; 
            mPlaybackState.AccumulatedTime = 0.0f;
        }
        ImGui::SameLine();

        const char* reverseButtonLabel = (mPlaybackState.bIsReversing || mPlaybackState.PlaybackSpeed < 0.0f) ? "Forward [▶>]": "Reverse [<◀]";
        if (ImGui::Button(reverseButtonLabel))
        {
            mPlaybackState.bIsReversing = !mPlaybackState.bIsReversing;
            // If speed is negative, make it positive for forward, and vice-versa, or just toggle bIsReversing and let speed sign control direction.
            // For simplicity, this button primarily toggles the bIsReversing flag. Speed sign can also dictate direction.
        }
        ImGui::SameLine();

        ImGui::Checkbox("Loop", &mPlaybackState.bIsLooping);
        ImGui::SameLine();

        ImGui::PushItemWidth(100.0f);
        if(ImGui::SliderFloat("Speed", &mPlaybackState.PlaybackSpeed, 0.0f, 4.0f, "%.1fx")) {
            // If speed becomes 0, effectively pause but keep isPlaying state unless explicitly stopped/paused.
            // If speed becomes negative, could imply reverse playing.
            if (mPlaybackState.PlaybackSpeed == 0.0f && mPlaybackState.bIsPlaying) {
                // mPlaybackState.bIsPaused = true; // Or handle as just zero speed.
            }
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushItemWidth(80.0f);
        if (ImGui::InputInt("Frame##Current", &mPlaybackState.CurrentFrame))
        {
            mPlaybackState.AccumulatedTime = 0.0f; 
            if (mPlaybackState.CurrentFrame > mPlaybackState.EndFrame) mPlaybackState.CurrentFrame = mPlaybackState.EndFrame;
            if (mPlaybackState.CurrentFrame < mPlaybackState.StartFrame) mPlaybackState.CurrentFrame = mPlaybackState.StartFrame;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("/ %d", mPlaybackState.EndFrame);

        ImGui::Spacing();
        ImGui::Text("Loop Range:"); ImGui::SameLine();
        ImGui::PushItemWidth(70.0f);
        if (ImGui::InputInt("Start##Loop", &mPlaybackState.LoopStartFrame)) {
             if (mPlaybackState.LoopStartFrame > mPlaybackState.LoopEndFrame) mPlaybackState.LoopStartFrame = mPlaybackState.LoopEndFrame;
             if (mPlaybackState.LoopStartFrame < mPlaybackState.StartFrame) mPlaybackState.LoopStartFrame = mPlaybackState.StartFrame;
             if (mPlaybackState.LoopStartFrame > mPlaybackState.EndFrame) mPlaybackState.LoopStartFrame = mPlaybackState.EndFrame;
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        ImGui::PushItemWidth(70.0f);
        if (ImGui::InputInt("End##Loop", &mPlaybackState.LoopEndFrame)) {
            if (mPlaybackState.LoopEndFrame < mPlaybackState.LoopStartFrame) mPlaybackState.LoopEndFrame = mPlaybackState.LoopStartFrame;
            if (mPlaybackState.LoopEndFrame > mPlaybackState.EndFrame) mPlaybackState.LoopEndFrame = mPlaybackState.EndFrame;
            if (mPlaybackState.LoopEndFrame < mPlaybackState.StartFrame) mPlaybackState.LoopEndFrame = mPlaybackState.StartFrame;
        }
        ImGui::PopItemWidth(); ImGui::SameLine();
        if(ImGui::Button("Set Start##BtnLoop")) { 
            mPlaybackState.LoopStartFrame = mPlaybackState.CurrentFrame; 
            if (mPlaybackState.LoopStartFrame > mPlaybackState.LoopEndFrame) mPlaybackState.LoopEndFrame = mPlaybackState.LoopStartFrame; 
        }
        ImGui::SameLine();
        if(ImGui::Button("Set End##BtnLoop")) { 
            mPlaybackState.LoopEndFrame = mPlaybackState.CurrentFrame; 
            if (mPlaybackState.LoopEndFrame < mPlaybackState.LoopStartFrame) mPlaybackState.LoopStartFrame = mPlaybackState.LoopEndFrame; 
        }

        if (mPlaybackState.bIsLooping)
        {
            bool crossedLoopEndForward = !mPlaybackState.bIsReversing && mPlaybackState.CurrentFrame > mPlaybackState.LoopEndFrame;
            bool crossedLoopStartReverse = mPlaybackState.bIsReversing && mPlaybackState.CurrentFrame < mPlaybackState.LoopStartFrame;
            if (crossedLoopEndForward)
            {
                mPlaybackState.CurrentFrame = mPlaybackState.LoopStartFrame + (mPlaybackState.CurrentFrame - mPlaybackState.LoopEndFrame -1); // Wrap around
                 if(mPlaybackState.CurrentFrame < mPlaybackState.LoopStartFrame) mPlaybackState.CurrentFrame = mPlaybackState.LoopStartFrame;
            }
            else if (crossedLoopStartReverse)
            {
                mPlaybackState.CurrentFrame = mPlaybackState.LoopEndFrame - (mPlaybackState.LoopStartFrame - mPlaybackState.CurrentFrame -1); // Wrap around
                if(mPlaybackState.CurrentFrame > mPlaybackState.LoopEndFrame) mPlaybackState.CurrentFrame = mPlaybackState.LoopEndFrame;
            }
            // Clamp to loop range if manually set outside or due to large step
            if (mPlaybackState.CurrentFrame > mPlaybackState.LoopEndFrame && mPlaybackState.LoopEndFrame >= mPlaybackState.LoopStartFrame) mPlaybackState.CurrentFrame = mPlaybackState.LoopEndFrame;
            if (mPlaybackState.CurrentFrame < mPlaybackState.LoopStartFrame && mPlaybackState.LoopEndFrame >= mPlaybackState.LoopStartFrame) mPlaybackState.CurrentFrame = mPlaybackState.LoopStartFrame;
        }
        else
        {
            if (!mPlaybackState.bIsReversing && mPlaybackState.CurrentFrame >= mPlaybackState.EndFrame)
            {
                mPlaybackState.CurrentFrame = mPlaybackState.EndFrame;
                if (mPlaybackState.bIsPlaying) { mPlaybackState.bIsPlaying = false; mPlaybackState.bIsPaused = false; } 
            }
            else if (mPlaybackState.bIsReversing && mPlaybackState.CurrentFrame <= mPlaybackState.StartFrame)
            {
                mPlaybackState.CurrentFrame = mPlaybackState.StartFrame;
                if (mPlaybackState.bIsPlaying) { mPlaybackState.bIsPlaying = false; mPlaybackState.bIsPaused = false; }
            }
        }
        
        if (mPlaybackState.CurrentFrame > mPlaybackState.EndFrame) mPlaybackState.CurrentFrame = mPlaybackState.EndFrame;
        if (mPlaybackState.CurrentFrame < mPlaybackState.StartFrame) mPlaybackState.CurrentFrame = mPlaybackState.StartFrame;

        // --- Update Skeletal Mesh Component Animation Time --- 
        if (RefSkeletalMeshComponent && (mPlaybackState.CurrentFrame != previousCurrentFrameForEngine || mPlaybackState.bIsPlaying || ImGui::IsAnyItemActive() ))
        {
            if (FrameRate > 0.0f) {
                float ElapsedTime = static_cast<float>(mPlaybackState.CurrentFrame) / FrameRate;
                // Using SetElapsedTime and SetCurrentKey as per original commented code and user feedback
                RefSkeletalMeshComponent->SetElapsedTime(ElapsedTime);
                // RefSkeletalMeshComponent->SetCurrentKey(mPlaybackState.CurrentFrame); // SetCurrentKey might be an internal helper or specific to a custom engine version.
                                                                            // SetElapsedTime is generally safer if available and works as expected.
                                                                            // If SetCurrentKey is indeed a valid public API for this engine version, it can be uncommented.
                                                                            // For now, relying on SetElapsedTime to also update the current key/frame implicitly or via internal logic.
                // To ensure the pose updates visually, a call to refresh or tick might be needed depending on the engine.
                // Example: RefSkeletalMeshComponent->ForceAnimationUpdate(); or similar.
                // If the component ticks itself based on world time, SetElapsedTime might be enough.
                // For an editor panel, we might need to explicitly tell it to update.
            }
            previousCurrentFrameForEngine = mPlaybackState.CurrentFrame; // Update for next tick comparison
        }
>>>>>>> cac7cb82 (UI 임시 작업중)
    }
    ImGui::End(); 
}

<<<<<<< HEAD

=======
>>>>>>> cac7cb82 (UI 임시 작업중)
FString BoneHierarchyViewerPanel::GetCleanBoneName(const FString& InFullName)
{
    int32 barIdx = InFullName.FindChar(TEXT('|'), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
    FString name = (barIdx != INDEX_NONE) ? InFullName.RightChop(barIdx + 1) : InFullName;

    int32 colonIdx = name.FindChar(TEXT(':'), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
    if (colonIdx != INDEX_NONE)
    {
        return name.RightChop(colonIdx + 1);
    }
    return name;
}

