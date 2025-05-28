#include "PhysicsViewerPanel.h"
#include "Engine/EditorEngine.h"

#include "Engine/Classes/Engine/SkeletalMesh.h"
#include "Engine/Classes/Animation/Skeleton.h"
#include "Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Engine/Asset/PhysicsAsset.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/ConstraintSetup.h"
#include "Components/BoxComponent.h"
#include "UObject/ObjectFactory.h"

PhysicsViewerPanel::PhysicsViewerPanel()
{
    SetSupportedWorldTypes(EWorldTypeBitFlag::PhysicsViewer);
}

void PhysicsViewerPanel::Render()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return;
    }

    if (BoneIconSRV == nullptr || NonWeightBoneIconSRV == nullptr) {
        LoadBoneIcon();
    }

    /* Pre Setup */
    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.7f;

    float PanelPosX = 5.0f;
    float PanelPosY = 5.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 1000);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar;


    if (Engine->ActiveWorld) {
        if (Engine->ActiveWorld->WorldType == EWorldType::PhysicsViewer) {

            if (CopiedRefSkeleton == nullptr) {
                CopyRefSkeleton(); // 선택된 액터/컴포넌트로부터 스켈레톤 정보 복사
            }

            // CopiedRefSkeleton이 여전히 null이면 렌더링하지 않음
            if (CopiedRefSkeleton == nullptr || CopiedRefSkeleton->RawRefBoneInfo.IsEmpty()) {
                ImGui::Begin("Bone Hierarchy", nullptr, PanelFlags); // 창은 표시하되 내용은 비움
                ImGui::Text("No skeleton selected or skeleton has no bones.");
                ImGui::End();
                return;
            }

            ImGui::Begin("Bone Hierarchy", nullptr, PanelFlags); // 창 이름 변경

            // 검색 필터 추가 (선택 사항)
            // static char BoneSearchText[128] = "";
            // ImGui::InputText("Search", BoneSearchText, IM_ARRAYSIZE(BoneSearchText));
            // FString SearchFilter(BoneSearchText);

            // 루트 본부터 시작하여 트리 렌더링
            for (int32 i = 0; i < CopiedRefSkeleton->RawRefBoneInfo.Num(); ++i)
            {
                if (CopiedRefSkeleton->RawRefBoneInfo[i].ParentIndex == INDEX_NONE) // 루트 본인 경우
                {
                    // RenderBoneTree 호출 시 Engine 포인터 전달
                    RenderBoneTree(*CopiedRefSkeleton, i, Engine /*, SearchFilter */);
                }
            }
            ImGui::End();
        }

        RenderConstraintPanel();

        RenderPhysicsDetailPanel();

        float ExitPanelWidth = (Width) * 0.2f - 6.0f;
        float ExitPanelHeight = 30.0f;

        const float margin = 10.0f;

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
            ClearRefSkeletalMeshComponent();
            UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine);
            EdEngine->EndPhysicsViewer();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void PhysicsViewerPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}

void PhysicsViewerPanel::SetSkeletalMesh(USkeletalMesh* SMesh)
{
    SkeletalMesh = SMesh;

	if (!SkeletalMesh->GetPhysicsAsset())
	{
		UPhysicsAsset* NewPhysicsAsset = FObjectFactory::ConstructObject<UPhysicsAsset>(nullptr);
		SkeletalMesh->SetPhysicsAsset(NewPhysicsAsset);
	}
}

int32 PhysicsViewerPanel::GetSelectedBoneIndex() const
{
    return SelectedBoneIndex;
}

FString PhysicsViewerPanel::GetSelectedBoneName() const
{
    if (SelectedBoneIndex == INDEX_NONE || !SkeletalMesh)
        return TEXT("");
    const auto& RefSkel = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();
    return RefSkel.RawRefBoneInfo[SelectedBoneIndex].Name.ToString();
}

void PhysicsViewerPanel::ClearRefSkeletalMeshComponent()
{
    if (RefSkeletalMeshComponent)
    {
        RefSkeletalMeshComponent = nullptr;
    }
    if (CopiedRefSkeleton)
    {
        CopiedRefSkeleton = nullptr;
    }
}

void PhysicsViewerPanel::LoadBoneIcon()
{
    BoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/Bone_16x.PNG")->TextureSRV;
    NonWeightBoneIconSRV = FEngineLoop::ResourceManager.GetTexture(L"Assets/Viewer/BoneNonWeighted_16x.PNG")->TextureSRV;
}

void PhysicsViewerPanel::CopyRefSkeleton()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    
	RefSkeletalMeshComponent = Engine->PhysicsViewerWorld->GetSkeletalMeshComponent();
	SetSkeletalMesh(RefSkeletalMeshComponent->GetSkeletalMeshAsset());
    const FReferenceSkeleton& OrigRef = SkeletalMesh->GetSkeleton()->GetReferenceSkeleton();

    CopiedRefSkeleton = new FReferenceSkeleton();
    CopiedRefSkeleton->RawRefBoneInfo = OrigRef.RawRefBoneInfo;
    CopiedRefSkeleton->RawRefBonePose = OrigRef.RawRefBonePose;
    CopiedRefSkeleton->InverseBindPoseMatrices = OrigRef.InverseBindPoseMatrices;
    CopiedRefSkeleton->RawNameToIndexMap = OrigRef.RawNameToIndexMap;
}

void PhysicsViewerPanel::RenderBoneTree(const FReferenceSkeleton& RefSkeleton, int32 BoneIndex, UEditorEngine* Engine /*, const FString& SearchFilter */)
{
    const FMeshBoneInfo& BoneInfo = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex];
    const FString& ShortBoneName = GetCleanBoneName(BoneInfo.Name.ToString());

    // 1) ImGui ID 충돌 방지
    ImGui::PushID(BoneIndex);

    ImGui::Image((ImTextureID)BoneIconSRV, ImVec2(16, 16));  // 16×16 픽셀 크기
    ImGui::SameLine();

    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
     if (SelectedBoneIndex == BoneIndex)
     {
         NodeFlags |= ImGuiTreeNodeFlags_Selected;
     }

    // 자식이 없는 본은 리프 노드로 처리 (화살표 없음)
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
        NodeFlags |= ImGuiTreeNodeFlags_Leaf; // 자식 없으면 리프 노드
        NodeFlags &= ~ImGuiTreeNodeFlags_OpenOnArrow; // 리프 노드는 화살표로 열 필요 없음
    }

    // ImGui::TreeNodeEx (본 이름, 플래그)
    // 이름 부분만 클릭 가능하도록 하려면 ImGui::Selectable을 함께 사용하거나 커스텀 로직 필요
    // 여기서는 TreeNodeEx 자체의 클릭 이벤트를 사용
    bool bNodeOpen = ImGui::TreeNodeEx(*ShortBoneName, NodeFlags);

    // --- 클릭 이벤트 처리 ---
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) // 왼쪽 마우스 버튼 클릭 시
    {
        // 엔진에 선택된 본 인덱스 설정 (가상의 함수 호출)
		SelectedBoneIndex = (BoneIndex);
    }

    if (bNodeOpen) // 노드가 열려있다면
    {
        // 자식 본들 재귀적으로 처리
        for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
        {
            if (RefSkeleton.RawRefBoneInfo[i].ParentIndex == BoneIndex)
            {
                RenderBoneTree(RefSkeleton, i, Engine /*, SearchFilter */); // 재귀 호출 시 Engine 전달
            }
        }
        ImGui::TreePop(); // 트리 노드 닫기
    }
    ImGui::PopID(); // ID 스택 복원
}

void PhysicsViewerPanel::RenderConstraintPanel()
{
    float ConstraintPanelWidth = (Width) * 0.2f - 6.0f;
    float ConstraintPanelHeight = (Height) * 0.25f;

    float ConstraintPanelPosX = 5.0f;
    float ConstraintPanelPosY = (Height) * 0.7f + 8.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 1000);

    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    ImGui::SetNextWindowPos(ImVec2(ConstraintPanelPosX, ConstraintPanelPosY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ConstraintPanelWidth, ConstraintPanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags DetailFlags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_HorizontalScrollbar;

    ImGui::Begin("Constraint Detail", nullptr, DetailFlags);

    if (SelectedBoneIndex == INDEX_NONE)
    {
        ImGui::Text("Select a bone to view constraint details");
        ImGui::End();
        return;
    }

    // 선택된 본 이름 가져오기
    if (!CopiedRefSkeleton || SelectedBoneIndex >= CopiedRefSkeleton->RawRefBoneInfo.Num())
    {
        ImGui::Text("Invalid selected bone");
        ImGui::End();
        return;
    }

    FName SelectedBoneName = CopiedRefSkeleton->RawRefBoneInfo[SelectedBoneIndex].Name;
    ImGui::Text("Selected Bone: %s", *SelectedBoneName.ToString());
    ImGui::Separator();

    // 선택된 본과 연결된 Constraint 찾기
    TArray<UConstraintSetup*> ConnectedConstraints;

    if (SkeletalMesh->GetPhysicsAsset() && SkeletalMesh->GetPhysicsAsset()->GetConstraintSetups().Num() > 0)
    {
        for (UConstraintSetup* Constraint : SkeletalMesh->GetPhysicsAsset()->GetConstraintSetups())
        {
            if (Constraint && (Constraint->BoneNameA == SelectedBoneName || Constraint->BoneNameB == SelectedBoneName))
            {
                ConnectedConstraints.Add(Constraint);
            }
        }
    }

    if (ConnectedConstraints.Num() == 0)
    {
        ImGui::Text("No constraints connected to this bone");
    }
    else
    {
        ImGui::Text("Connected Constraints (%d):", ConnectedConstraints.Num());
        ImGui::Separator();

        // 각 Constraint를 트리 노드로 표시
        for (int32 i = 0; i < ConnectedConstraints.Num(); ++i)
        {
            UConstraintSetup* Constraint = ConnectedConstraints[i];

            ImGui::PushID(i);

            // Constraint 이름 생성 (BoneA -> BoneB 형태)
            FString ConstraintName = FString::Printf(TEXT("Constraint %d: %s -> %s"),
                i + 1,
                *Constraint->BoneNameA.ToString(),
                *Constraint->BoneNameB.ToString());

            if (ImGui::TreeNode(GetData(ConstraintName)))
            {
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    ImGui::End();
}

void PhysicsViewerPanel::RenderPhysicsDetailPanel()
{
    float DetailPanelWidth = Width * 0.2f - 6.0f;
    float DetailPanelHeight = Height * 0.9f;
    float DetailPanelPosX = Width * 0.8f;
    float DetailPanelPosY = 5.0f;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, 1000);

    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    ImGui::SetNextWindowPos(ImVec2(DetailPanelPosX, DetailPanelPosY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(DetailPanelWidth, DetailPanelHeight), ImGuiCond_Always);

    constexpr ImGuiWindowFlags DetailFlags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_HorizontalScrollbar;

    ImGui::Begin("Physics Details", nullptr, DetailFlags);

	if (ImGui::Button("Generate Box Bodies for All Bones"))
	{
		//GenerateBoxBodiesForAllBones();
        //GenerateConstraintsForAllBones();
        GenerateRagdoll();
	}

	ImGui::Separator();

	// No Selected Bone (Shape)
	if (SelectedBoneIndex == INDEX_NONE)
	{
		ImGui::Text("Select a bone to view physics details");
		ImGui::End();
		return;
	}

	if (CopiedRefSkeleton && SelectedBoneIndex < CopiedRefSkeleton->RawRefBoneInfo.Num())
	{
		const FMeshBoneInfo& BoneInfo = CopiedRefSkeleton->RawRefBoneInfo[SelectedBoneIndex];
		ImGui::Text("Selected Bone: %s", *BoneInfo.Name.ToString());
		ImGui::Separator();

        FTransform BoneTransform = CalculateBoneWorldTransform(SelectedBoneIndex);
		
        // 선택된 본의 BodySetup 찾기
		UBodySetup* CurrentBodySetup = FindBodySetupForBone(SelectedBoneIndex);
        UConstraintSetup* CurrentConstraintSetup = FindConstraintSetupForBone(SelectedBoneIndex);
		
        if (CurrentBodySetup)
		{
			RenderBodySetupEditor(CurrentBodySetup);
		}
		else
		{
			ImGui::Text("No Physics Body for this bone");
			ImGui::Separator();

			if (ImGui::Button("Create Physics Body"))
			{
                CreatePhysicsBodySetup(BoneInfo, BoneTransform, SelectedBoneIndex);
			}
		}

        if (CurrentConstraintSetup)
        {
            RenderConstrainSetupEditor(CurrentConstraintSetup);
        }
	}

	ImGui::End();
}

void PhysicsViewerPanel::RenderBodySetupEditor(UBodySetup* BodySetup)
{
	if (!BodySetup)
	{
		return;
	}

	ImGui::Text("Physics BodySetup: %s", *BodySetup->BoneName.ToString());
	ImGui::Separator();

	if (BodySetup)
	{
		BodySetup->DisplayProperty();
	}
}

void PhysicsViewerPanel::RenderConstrainSetupEditor(UConstraintSetup* ConstraintSetup)
{
    if (!ConstraintSetup)
    {
        return;
    }

    ImGui::Text("Constraint BoneA: %s", *ConstraintSetup->BoneNameA.ToString());
    ImGui::Text("Constraint BoneB: %s", *ConstraintSetup->BoneNameB.ToString());
    ImGui::Separator();

    if (ConstraintSetup)
    {
        ConstraintSetup->DisplayProperty();
    }
}

UBodySetup* PhysicsViewerPanel::FindBodySetupForBone(int32 BoneIndex)
{
	if (!SkeletalMesh->GetPhysicsAsset() || !CopiedRefSkeleton || BoneIndex >= CopiedRefSkeleton->RawRefBoneInfo.Num())
	{
		return nullptr;
	}

	FName BoneName = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex].Name;

	for (UBodySetup* BodySetup : SkeletalMesh->GetPhysicsAsset()->GetBodySetups())
	{
		if (BodySetup && BodySetup->BoneName == BoneName)
		{
			return BodySetup;
		}
	}

	return nullptr;
}

UConstraintSetup* PhysicsViewerPanel::FindConstraintSetupForBone(int32 BoneIndex)
{
    if (!SkeletalMesh->GetPhysicsAsset() || !CopiedRefSkeleton || BoneIndex >= CopiedRefSkeleton->RawRefBoneInfo.Num())
    {
        return nullptr;
    }

    FName BoneName = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex].Name;

    for (UConstraintSetup* ConstraintSetups : SkeletalMesh->GetPhysicsAsset()->GetConstraintSetups())
    {
        if (ConstraintSetups && ConstraintSetups->BoneNameA == BoneName)
        {
            return ConstraintSetups;
        }
    }

    return nullptr;
}
FString PhysicsViewerPanel::GetCleanBoneName(const FString& InFullName)
{
    int32 barIdx = InFullName.FindChar(TEXT('|'),
        /*case*/ ESearchCase::CaseSensitive,
        /*dir*/  ESearchDir::FromEnd);
    FString name = (barIdx != INDEX_NONE)
        ? InFullName.RightChop(barIdx + 1)
        : InFullName;

    int32 colonIdx = name.FindChar(TEXT(':'),
        /*case*/ ESearchCase::CaseSensitive,
        /*dir*/  ESearchDir::FromEnd);
    if (colonIdx != INDEX_NONE)
    {
        return name.RightChop(colonIdx + 1);
    }
    return name;
}

// SkeletalMesh의 Bone마다 BoxShape를 할당하여 초기화
void PhysicsViewerPanel::GenerateBoxBodiesForAllBones()
{
	UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
	if (!CopiedRefSkeleton || !Engine->PhysicsViewerWorld)
	{
		return;
	}

	// 추후 초기화 작업 등 추가
	//ClearExistingBoxComponents();

	// 각 본에 대해 Shape 생성
    for (int32 BoneIndex = 0; BoneIndex < CopiedRefSkeleton->RawRefBoneInfo.Num(); ++BoneIndex)
    {
        const FMeshBoneInfo& BoneInfo = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex];

        FTransform BoneTransform = CalculateBoneWorldTransform(BoneIndex);

        CreatePhysicsBodySetup(BoneInfo, BoneTransform, BoneIndex);

        UBodySetup* BodySetup = FindBodySetupForBone(BoneIndex);

        if (BoneInfo.ParentIndex == -1) continue; // 루트 본은 스킵

        FVector ParentLocation = CalculateBoneWorldTransform(BoneInfo.ParentIndex).GetTranslation();
        FVector BoneLocation = CalculateBoneWorldTransform(BoneIndex).GetTranslation();

        float BoneLength = (BoneLocation - ParentLocation).Length();

        // CAPSULE TEST
        FKSphylElem SphylElem;
        // 원본 Length = half length * 2 + 2 * radius
        SphylElem.Center = FVector::ZeroVector;
        SphylElem.Radius = BoneLength * 0.5f * 0.2f;
        SphylElem.Length = (BoneLength - SphylElem.Radius * 2) * 0.5f;
        SphylElem.Rotation = FRotator::ZeroRotator;
        SphylElem.Center.Z = BoneLength * -0.5f;
        BodySetup->AggGeom.SphylElems.Add(SphylElem);
    }
}

void PhysicsViewerPanel::GenerateConstraintsForAllBones()
{
    for (int32 i = 0; i < CopiedRefSkeleton->RawRefBoneInfo.Num(); ++i)
    {
        const auto& Info = CopiedRefSkeleton->RawRefBoneInfo[i];
        if (Info.ParentIndex == INDEX_NONE) continue;

        UBodySetup* BodyA = FindBodySetupForBone(Info.ParentIndex);
        UBodySetup* BodyB = FindBodySetupForBone(i);
        if (!BodyA || !BodyB) continue;

        UConstraintSetup* C = FObjectFactory::ConstructObject<UConstraintSetup>(SkeletalMesh->GetPhysicsAsset());
        C->BoneNameA = BodyA->BoneName;
        C->BoneNameB = BodyB->BoneName;

        // 개선된 로컬 프레임 계산
        CalculateConstraintTransforms(Info.ParentIndex, i, C);

        C->SwingLimitAngle = 45.0f;   // 필요하면 조정
        C->TwistLimitAngle = 30.0f;   // 필요하면 조정
        SkeletalMesh->GetPhysicsAsset()->GetConstraintSetups().Add(C);
    }
}

void PhysicsViewerPanel::GenerateRagdoll()
{
	if (!RefSkeletalMeshComponent->GetPhysicsAsset())
	{
		return;
	}

	if (!CopiedRefSkeleton)
	{
		CopyRefSkeleton();
	}

	GenerateRagdollBody();
	GenerateRagdollConstraint();
}

void PhysicsViewerPanel::GenerateRagdollBody()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!CopiedRefSkeleton || !Engine->PhysicsViewerWorld)
    {
        return;
    }

    TArray<FMatrix> CurrentGlobalBoneMatrix;
    RefSkeletalMeshComponent->GetCurrentGlobalBoneMatrices(CurrentGlobalBoneMatrix);

    FTransform WorldTransform = RefSkeletalMeshComponent->GetWorldTransform();

    for (int32 CurBoneIndex = 0; CurBoneIndex < CopiedRefSkeleton->RawRefBoneInfo.Num(); CurBoneIndex++)
    {
        const FMeshBoneInfo& BoneInfo = CopiedRefSkeleton->RawRefBoneInfo[CurBoneIndex];
        int32 ParentIndex = BoneInfo.ParentIndex;
        if (ParentIndex == INDEX_NONE || ParentIndex == 0)
        {
            continue;
        }

        UBodySetup* NewBodySetup = FObjectFactory::ConstructObject<UBodySetup>(SkeletalMesh->GetPhysicsAsset());
        NewBodySetup->BoneName = CopiedRefSkeleton->RawRefBoneInfo[ParentIndex].Name;
        NewBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
        SkeletalMesh->GetPhysicsAsset()->GetBodySetups().Add(NewBodySetup);
        
        FTransform ParentWorld = FTransform(CurrentGlobalBoneMatrix[ParentIndex]);
        FTransform ChildWorld = FTransform(CurrentGlobalBoneMatrix[CurBoneIndex]);

        float BoneLength = (CopiedRefSkeleton->GetRawRefBonePose()[CurBoneIndex].GetTranslation()).Length();

        FVector ParentLoc = ParentWorld.GetTranslation();
        FVector ChildLoc = ChildWorld.GetTranslation();

        //BoneLength = (ChildLoc - ParentLoc).Length();
        FVector Dir = (ChildLoc - ParentLoc).GetSafeNormal();

        const FVector Up(0, 0, 1);
        float Dot = FMath::Clamp(FVector::DotProduct(Up, Dir), -1.0f, 1.0f);
        FVector Axis = Up ^ Dir;
        if (Axis.IsNearlyZero())
        {
            Axis = FVector(1, 0, 0);
        }
        Axis.Normalize();
        float Angle = FMath::Acos(Dot);
        FQuat RotQuat(Axis, Angle);
        
        FKSphylElem SphylElem;
        // 원본 Length = half length * 2 + 2 * radius
        SphylElem.Center = FVector::ZeroVector;
        //SphylElem.Radius = BoneLength * 0.1f;
        SphylElem.Radius = FMath::Min((BoneLength - SphylElem.Radius * 2) * 0.5f, 2.0f);
        SphylElem.Length = (BoneLength - SphylElem.Radius * 2) * 0.5f;
        //SphylElem.Length = (BoneLength * 0.4f);
        SphylElem.Rotation = RotQuat.Rotator();
        SphylElem.Center.Z = BoneLength * 0.5f;
        NewBodySetup->AggGeom.SphylElems.Add(SphylElem);
    }
}

void PhysicsViewerPanel::GenerateRagdollConstraint()
{
}

void PhysicsViewerPanel::CreateBodySetupForAllBone()
{
}

void PhysicsViewerPanel::CalculateConstraintTransforms(int32 ParentBoneIndex, int32 ChildBoneIndex, UConstraintSetup* ConstraintSetup)
{
    //FTransform WorldA = CalculateBoneWorldTransform(ParentBoneIndex);
    //FTransform WorldB = CalculateBoneWorldTransform(ChildBoneIndex);

    //// 참조 포즈의 부모→자식 오프셋 벡터
    //FVector RefOffset =
    //    CopiedRefSkeleton->RawRefBonePose[ChildBoneIndex].GetTranslation()
    //    - CopiedRefSkeleton->RawRefBonePose[ParentBoneIndex].GetTranslation();

    //// 월드 좌표로 변환
    //FVector JointWorldPos = WorldA.TransformPosition(RefOffset);

    //FTransform JointWorldTransform(
    //    WorldA.GetRotation(),  // Joint 회전: 부모 본 축을 따른다
    //    JointWorldPos,
    //    FVector::OneVector     // 스케일은 1:1
    //);

    //// 동일한 JointWorldPos 를 기준으로 로컬 프레임 설정
    //ConstraintSetup->TransformInA = JointWorldTransform.GetRelativeTransform(WorldA);
    //ConstraintSetup->TransformInB = JointWorldTransform.GetRelativeTransform(WorldB);

    // 본의 레퍼런스 포즈 가져오기
    const FTransform& ParentRefPose = CopiedRefSkeleton->RawRefBonePose[ParentBoneIndex];
    const FTransform& ChildRefPose = CopiedRefSkeleton->RawRefBonePose[ChildBoneIndex];

    // 부모 본의 BodySetup에서 박스 크기 가져오기
    UBodySetup* ParentBodySetup = FindBodySetupForBone(ParentBoneIndex);
    float ParentBoneHalfLength = 2.0f; // 기본값
    if (ParentBodySetup && ParentBodySetup->AggGeom.SphylElems.Num() > 0)
    {
        ParentBoneHalfLength = ParentBodySetup->AggGeom.SphylElems[0].Radius + ParentBodySetup->AggGeom.SphylElems[0].Length;
    }

    // TransformInA: 부모 본의 로컬 좌표계에서 관절까지의 Transform
    // 일반적으로 부모 본의 끝부분에 관절이 위치
    FVector JointLocationInParent = FVector(0, 0, ParentBoneHalfLength); // 본의 끝부분
    ConstraintSetup->TransformInA = FTransform(
        FRotator::ZeroRotator,      // 부모 본의 기본 방향 사용
        JointLocationInParent,      // 부모 본 끝부분
        FVector::OneVector          // 스케일 1,1,1
    );

    // TransformInB: 자식 본의 로컬 좌표계에서 관절까지의 Transform
    // 일반적으로 자식 본의 시작점에 관절이 위치
    ConstraintSetup->TransformInB = FTransform(
        FRotator::ZeroRotator,      // 자식 본의 기본 방향 사용
        FVector::ZeroVector,        // 자식 본의 시작점
        FVector::OneVector          // 스케일 1,1,1
    );
}

FTransform PhysicsViewerPanel::CalculateBoneWorldTransform(int32 BoneIndex)
{
	if (!CopiedRefSkeleton || BoneIndex >= CopiedRefSkeleton->RawRefBoneInfo.Num())
	{
		return FTransform::Identity;
	}

	FTransform BoneTransform = CopiedRefSkeleton->RawRefBonePose[BoneIndex];

	int32 ParentIndex = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex].ParentIndex;
	while (ParentIndex != INDEX_NONE)
	{
		FTransform ParentTransform = CopiedRefSkeleton->RawRefBonePose[ParentIndex];
		BoneTransform = ParentTransform * BoneTransform;
		ParentIndex = CopiedRefSkeleton->RawRefBoneInfo[ParentIndex].ParentIndex;
	}

	return BoneTransform;
}

void PhysicsViewerPanel::CreatePhysicsBodySetup(const FMeshBoneInfo& BoneInfo, const FTransform& BoneTransform, int32 BoneIndex)
{
	if (!SkeletalMesh->GetPhysicsAsset()) 
	{
		UPhysicsAsset* NewPhysicsAsset = FObjectFactory::ConstructObject<UPhysicsAsset>(nullptr);
		SkeletalMesh->SetPhysicsAsset(NewPhysicsAsset);
	}
	UBodySetup* NewBodySetup = FObjectFactory::ConstructObject<UBodySetup>(SkeletalMesh->GetPhysicsAsset());
	NewBodySetup->BoneName = BoneInfo.Name;

	NewBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
	//NewBodySetup->DefaultInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//NewBodySetup->DefaultInstance.SetObjectType(ECollisionChannel::ECC_WorldDynamic);

	SkeletalMesh->GetPhysicsAsset()->GetBodySetups().Add(NewBodySetup);
}

void PhysicsViewerPanel::CreateBoxComponentForBone(int32 BoneIndex, const FTransform& BoneTransform, const FName& BoneName)
{
	UBoxComponent* BoxComponent = RefSkeletalMeshComponent->GetOwner()->AddComponent<UBoxComponent>();
	// 박스 크기 설정
	// 초기에 적절한 사이즈를 주는 방법은 추후 고려
	FVector BoxExtent(1.0f, 1.0f, 1.0f);
	BoxComponent->SetBoxExtent(BoxExtent);

	// 위치와 회전 설정
	BoxComponent->SetWorldTransform(BoneTransform);
}

void PhysicsViewerPanel::CreateHierarchicalBoxComponent(int32 BoneIndex, const FTransform& BoneLocalTransform, const FName& BoneName)
{
    if (!CopiedRefSkeleton || BoneIndex >= CopiedRefSkeleton->RawRefBoneInfo.Num())
    {
        return;
    }

    const FMeshBoneInfo& BoneInfo = CopiedRefSkeleton->RawRefBoneInfo[BoneIndex];

    // 부모 컴포넌트 찾기
    USceneComponent* ParentComponent = nullptr;

    if (BoneInfo.ParentIndex != INDEX_NONE)
    {
        // 부모 본의 컴포넌트 찾기
        if (USceneComponent** ParentCompPtr = BoneComponentMap.Find(BoneInfo.ParentIndex))
        {
            ParentComponent = *ParentCompPtr;
        }
        else
        {
            // 부모 컴포넌트가 아직 생성되지 않은 경우, 먼저 생성
            CreateHierarchicalBoxComponent(BoneInfo.ParentIndex,
                CopiedRefSkeleton->RawRefBonePose[BoneInfo.ParentIndex],
                CopiedRefSkeleton->RawRefBoneInfo[BoneInfo.ParentIndex].Name);

            if (USceneComponent** ParentCompPtr = BoneComponentMap.Find(BoneInfo.ParentIndex))
            {
                ParentComponent = *ParentCompPtr;
            }
        }
    }
    else
    {
        // 루트 본인 경우 SkeletalMeshComponent를 부모로 사용
        ParentComponent = RefSkeletalMeshComponent;
    }

    // 이미 생성된 컴포넌트가 있는지 확인
    if (BoneComponentMap.Contains(BoneIndex))
    {
        return;
    }

    // BoxComponent 생성
    UBoxComponent* BoxComponent = RefSkeletalMeshComponent->GetOwner()->AddComponent<UBoxComponent>();

    FVector BoxExtent(1.0f, 1.0f, 1.0f);
    BoxComponent->SetBoxExtent(BoxExtent);

    // 부모에 연결
    if (ParentComponent)
    {
        BoxComponent->AttachToComponent(ParentComponent);
    }

    // 로컬 Transform 설정 (부모 기준 상대적 위치)
    BoxComponent->SetRelativeTransform(BoneLocalTransform);

    // 매핑 테이블에 추가
    BoneComponentMap.Add(BoneIndex, BoxComponent);
}
