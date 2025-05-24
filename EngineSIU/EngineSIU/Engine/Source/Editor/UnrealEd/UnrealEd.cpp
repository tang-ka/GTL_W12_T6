#include "UnrealEd.h"
#include "EditorPanel.h"

#include "PropertyEditor/ControlEditorPanel.h"
#include "PropertyEditor/OutlinerEditorPanel.h"
#include "PropertyEditor/ParticleViewerPanel.h"
#include "PropertyEditor/PropertyEditorPanel.h"
#include "PropertyEditor/SkeletalMeshViewerPanel.h"
#include "PropertyEditor/PhysicsViewerPanel.h"

#include "World/World.h"
void UnrealEd::Initialize()
{
    auto ControlPanel = std::make_shared<ControlEditorPanel>();
    Panels["ControlPanel"] = ControlPanel;
    
    auto OutlinerPanel = std::make_shared<OutlinerEditorPanel>();
    Panels["OutlinerPanel"] = OutlinerPanel;
    
    auto PropertyPanel = std::make_shared<PropertyEditorPanel>();
    Panels["PropertyPanel"] = PropertyPanel;

    auto BoneHierarchyPanel = std::make_shared<SkeletalMeshViewerPanel>();
    Panels["BoneHierarchyPaenl"] = BoneHierarchyPanel;
    
    auto ParticleViewPanel = std::make_shared<ParticleViewerPanel>();
    Panels["ParticleViewerPanel"] = ParticleViewPanel;

    auto PhysicsViewrPanel = std::make_shared<PhysicsViewerPanel>();
    Panels["PhysicsViewrPanel"] = PhysicsViewrPanel;
}

void UnrealEd::Render() const
{
    EWorldTypeBitFlag currentMask;
    switch (GEngine->ActiveWorld->WorldType)
    {
    case EWorldType::Game:
        currentMask = EWorldTypeBitFlag::Game;
        break;
    case EWorldType::Editor:
        currentMask = EWorldTypeBitFlag::Editor;
        break;
    case EWorldType::PIE:
        currentMask = EWorldTypeBitFlag::PIE;
        break;
    case EWorldType::EditorPreview:
        currentMask = EWorldTypeBitFlag::EditorPreview;
        break;
    case EWorldType::GamePreview:
        currentMask = EWorldTypeBitFlag::GamePreview;
        break;
    case EWorldType::GameRPC:
        currentMask = EWorldTypeBitFlag::GameRPC;
        break;
    case EWorldType::SkeletalViewer:
        currentMask = EWorldTypeBitFlag::SkeletalViewer;
        break;
    case EWorldType::ParticleViewer:
        currentMask = EWorldTypeBitFlag::ParticleViewer;
        break;
    case EWorldType::PhysicsViewer:
        currentMask = EWorldTypeBitFlag::PhysicsViewer;
        break;
    case EWorldType::Inactive:
        currentMask = EWorldTypeBitFlag::Inactive;
        break;
    default:
        currentMask = EWorldTypeBitFlag::None;
        break;
        
    }
    for (const auto& Panel : Panels)
    {
        if (HasFlag(Panel.Value->GetSupportedWorldTypes(), currentMask))
        {
            Panel.Value->Render();
        }
    }
}

void UnrealEd::AddEditorPanel(const FString& PanelId, const std::shared_ptr<UEditorPanel>& EditorPanel)
{
    Panels[PanelId] = EditorPanel;
}

void UnrealEd::OnResize(HWND hWnd) const
{
    for (auto& Panel : Panels)
    {
        Panel.Value->OnResize(hWnd);
    }
}

std::shared_ptr<UEditorPanel> UnrealEd::GetEditorPanel(const FString& PanelId)
{
    return Panels[PanelId];
}
