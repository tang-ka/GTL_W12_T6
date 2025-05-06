
#include "FbxLoader.h"
#include "FbxLoader.h"

#include <format>

#include "AssetManager.h"
#include "Asset/SkeletalMeshAsset.h"
#include "UObject/ObjectFactory.h"
#include "Math/transform.h"
#include "Animation/Skeleton.h"
#include "SkeletalMesh.h"

struct FVertexKey
{
    int32 PositionIndex;
    int32 NormalIndex;
    int32 TangentIndex;
    int32 UVIndex;
    int32 ColorIndex;

    FVertexKey(int32 Pos, int32 N, int32 T, int32 UV, int32 Col)
        : PositionIndex(Pos)
        , NormalIndex(N)
        , TangentIndex(T)
        , UVIndex(UV)
        , ColorIndex(Col)
    {
        Hash = std::hash<int32>()(PositionIndex << 0)
             ^ std::hash<int32>()(NormalIndex   << 1)
             ^ std::hash<int32>()(TangentIndex  << 2)
             ^ std::hash<int32>()(UVIndex       << 3)
             ^ std::hash<int32>()(ColorIndex    << 4);
    }

    bool operator==(const FVertexKey& Other) const
    {
        return PositionIndex == Other.PositionIndex
            && NormalIndex   == Other.NormalIndex
            && TangentIndex  == Other.TangentIndex
            && UVIndex       == Other.UVIndex
            && ColorIndex    == Other.ColorIndex;
    }

    SIZE_T GetHash() const { return Hash; }

private:
    SIZE_T Hash;
};

namespace std
{
    template<>
    struct hash<FVertexKey>
    {
        size_t operator()(const FVertexKey& Key) const
        {
            return Key.GetHash();
        }
    };
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 XYZ로 변환 (좌표계 변환 포함)
inline void SetVertexPosition(FSkeletalMeshVertex& Vertex, const FbxVector4& Pos)
{
    Vertex.X = static_cast<float>(Pos[0]);
    Vertex.Y = static_cast<float>(Pos[1]);
    Vertex.Z = static_cast<float>(Pos[2]);
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 Normal XYZ로 변환 (좌표계 변환 포함)
inline void SetVertexNormal(FSkeletalMeshVertex& Vertex, const FbxVector4& Normal)
{
    Vertex.NormalX = static_cast<float>(Normal[0]);
    Vertex.NormalY = static_cast<float>(Normal[1]);
    Vertex.NormalZ = static_cast<float>(Normal[2]);
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 Tangent XYZW로 변환 (좌표계 변환 포함)
inline void SetVertexTangent(FSkeletalMeshVertex& Vertex, const FbxVector4& Tangent)
{
    Vertex.TangentX = static_cast<float>(Tangent[0]);
    Vertex.TangentY = static_cast<float>(Tangent[1]);
    Vertex.TangentZ = static_cast<float>(Tangent[2]);
    Vertex.TangentW = static_cast<float>(Tangent[3]); // W (Handedness)
}

// 헬퍼 함수: FbxColor를 FSkeletalMeshVertex의 RGBA로 변환
inline void SetVertexColor(FSkeletalMeshVertex& Vertex, const FbxColor& Color)
{
    Vertex.R = static_cast<float>(Color.mRed);
    Vertex.G = static_cast<float>(Color.mGreen);
    Vertex.B = static_cast<float>(Color.mBlue);
    Vertex.A = static_cast<float>(Color.mAlpha);
}

// 헬퍼 함수: FbxVector2를 FSkeletalMeshVertex의 UV로 변환 (좌표계 변환 포함)
inline void SetVertexUV(FSkeletalMeshVertex& Vertex, const FbxVector2& UV)
{
    Vertex.U = static_cast<float>(UV[0]);
    Vertex.V = 1.0f - static_cast<float>(UV[1]); // V 좌표는 보통 뒤집힘 (DirectX 스타일)
}

// FbxLayerElementTemplate에서 데이터를 가져오는 일반화된 헬퍼 함수
template<typename FbxLayerElementType, typename TDataType>
bool GetVertexElementData(const FbxLayerElementType* Element, int32 ControlPointIndex, int32 VertexIndex, TDataType& OutData)
{
    if (!Element)
    {
        return false;
    }

    const auto MappingMode = Element->GetMappingMode();
    const auto ReferenceMode = Element->GetReferenceMode();

    // eAllSame: 모든 정점이 같은 값
    if (MappingMode == FbxLayerElement::eAllSame)
    {
        if (Element->GetDirectArray().GetCount() > 0)
        {
            OutData = Element->GetDirectArray().GetAt(0);
            return true;
        }
        return false;
    }

    // 2) 인덱스 결정 (eByControlPoint, eByPolygonVertex만 처리)
    int32 Index = -1;
    if (MappingMode == FbxLayerElement::eByControlPoint)
    {
        Index = ControlPointIndex;
    }
    else if (MappingMode == FbxLayerElement::eByPolygonVertex)
    {
        Index = VertexIndex;
    }
    else
    {
        // eByPolygon, eByEdge 등 필요시 추가
        return false;
    }

    // 3) ReferenceMode별 분리 처리
    if (ReferenceMode == FbxLayerElement::eDirect)
    {
        // DirectArray 크기만 검사
        if (Index >= 0 && Index < Element->GetDirectArray().GetCount())
        {
            OutData = Element->GetDirectArray().GetAt(Index);
            return true;
        }
    }
    else if (ReferenceMode == FbxLayerElement::eIndexToDirect)
    {
        // IndexArray, DirectArray 순차 검사
        if (Index >= 0 && Index < Element->GetIndexArray().GetCount())
        {
            int32 DirectIndex = Element->GetIndexArray().GetAt(Index);
            if (DirectIndex >= 0 && DirectIndex < Element->GetDirectArray().GetCount())
            {
                OutData = Element->GetDirectArray().GetAt(DirectIndex);
                return true;
            }
        }
    }

    return false;
}

FFbxLoader::FFbxLoader()
    : Manager(nullptr)
    , Importer(nullptr)
    , Scene(nullptr)
{
    Manager = FbxManager::Create();

    FbxIOSettings* IOSettings = FbxIOSettings::Create(Manager, IOSROOT);
    Manager->SetIOSettings(IOSettings);

    IOSettings->SetBoolProp(IMP_FBX_MATERIAL, true);
    IOSettings->SetBoolProp(IMP_FBX_TEXTURE, true);
    IOSettings->SetBoolProp(IMP_FBX_ANIMATION, true);
    
    Importer = FbxImporter::Create(Manager, "");
    Scene = FbxScene::Create(Manager, "");
}

FFbxLoader::~FFbxLoader()
{
    if (Scene)
    {
        Scene->Destroy();
    }
    if (Importer)
    {
        Importer->Destroy();
    }
    if (Manager)
    {
        Manager->Destroy();
    }
}

void PrintNodeAttribute(FbxNode* Node, int32 Depth)
{
    if (!Node)
    {
        return;
    }

    for (int32 i = 0; i < Depth; ++i)
    {
        OutputDebugStringA("    ");
    }
    OutputDebugStringA("--  [");
    OutputDebugStringA(Node->GetName());
    OutputDebugStringA("]: ");

    if (auto Attr = Node->GetNodeAttribute())
    {
        if (Attr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            OutputDebugStringA("Skeleton");
            if (FbxSkeleton* Skeleton = Node->GetSkeleton())
            {
                if (Skeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
                {
                    OutputDebugStringA(", LimbNode");
                }
                else if (Skeleton->GetSkeletonType() == FbxSkeleton::eRoot)
                {
                    OutputDebugStringA(", Root");
                }
                else if (Skeleton->GetSkeletonType() == FbxSkeleton::eLimb)
                {
                    OutputDebugStringA(", Limb");
                }
            }
        }
        else if (Attr->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            OutputDebugStringA("Mesh");
        }
        else
        {
            OutputDebugStringA("Unknown");
        }
    }
    OutputDebugStringA("\n");

    for (int32 i = 0; i < Node->GetChildCount(); ++i)
    {
        PrintNodeAttribute(Node->GetChild(i), Depth + 1);
    }
}

FFbxLoadResult FFbxLoader::LoadFBX(const FString& InFilePath)
{
    bool bSuccess = false;
    if (Importer->Initialize(*InFilePath, -1, Manager->GetIOSettings()))
    {
        bSuccess = Importer->Import(Scene);
    }
    if (!bSuccess)
    {
        return std::move(FFbxLoadResult());
    }

    ConvertSceneToLeftHandedZUpXForward(Scene);

    const FbxGlobalSettings& GlobalSettings = Scene->GetGlobalSettings();
    FbxSystemUnit SystemUnit = GlobalSettings.GetSystemUnit();
    const double ScaleFactor = SystemUnit.GetScaleFactor();
    OutputDebugStringA(std::format("### FBX ###\nScene Scale: {} cm\n", ScaleFactor).c_str());

    FbxNode* RootNode = Scene->GetRootNode();
    if (!RootNode)
    {
        return std::move(FFbxLoadResult());
    }

    FFbxLoadResult Result;
    
    FbxGeometryConverter Converter(Manager);
    Converter.Triangulate(Scene, true);

    PrintNodeAttribute(RootNode, 0);

    ProcessMaterials(Result);

    ProcessSkeletonHierarchy(RootNode, Result);

    ProcessMeshesWithSkeletons(RootNode, Result);
    
    return Result;
}

void FFbxLoader::ProcessMaterials(FFbxLoadResult& OutResult)
{
    const int32 MaterialCount = Scene->GetMaterialCount();

    for (int32 i = 0; i < MaterialCount; ++i)
    {
        FbxSurfaceMaterial* FbxMaterial = Scene->GetMaterial(i);
        if (!FbxMaterial)
        {
            continue;
        }

        FObjMaterialInfo MaterialInfo = ExtractMaterialsFromFbx(FbxMaterial);

        UMaterial* NewMaterial = FObjectFactory::ConstructObject<UMaterial>(nullptr, FbxMaterial->GetName());
        NewMaterial->SetMaterialInfo(MaterialInfo);

        OutResult.Materials.Add(NewMaterial);
    }
}

FObjMaterialInfo FFbxLoader::ExtractMaterialsFromFbx(FbxSurfaceMaterial* FbxMaterial)
{
    FObjMaterialInfo MaterialInfo = {};
    
    if (!FbxMaterial)
    {
        return MaterialInfo;
    }

    MaterialInfo.MaterialName = FbxMaterial->GetName();

    if (FbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
    {
        FbxSurfaceLambert* Lambert = static_cast<FbxSurfaceLambert*>(FbxMaterial);
        
        FbxDouble3 Diffuse = Lambert->Diffuse.Get();
        MaterialInfo.DiffuseColor = FVector(
            static_cast<float>(Diffuse[0]), 
            static_cast<float>(Diffuse[1]), 
            static_cast<float>(Diffuse[2])
        );
        
        FbxDouble3 Ambient = Lambert->Ambient.Get();
        MaterialInfo.AmbientColor = FVector(
            static_cast<float>(Ambient[0]), 
            static_cast<float>(Ambient[1]), 
            static_cast<float>(Ambient[2])
        );
        
        FbxDouble3 Emissive = Lambert->Emissive.Get();
        MaterialInfo.EmissiveColor = FVector(
            static_cast<float>(Emissive[0]), 
            static_cast<float>(Emissive[1]), 
            static_cast<float>(Emissive[2])
        );
        
        // 투명도 처리
        float Transparency = static_cast<float>(1.0 - Lambert->TransparencyFactor.Get());
        MaterialInfo.Transparency = Transparency;
        MaterialInfo.bTransparent = (Transparency < 1.0f);
    }
    
    // Phong 머티리얼 추가 속성 (Lambert를 상속함)
    if (FbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
    {
        FbxSurfacePhong* Phong = static_cast<FbxSurfacePhong*>(FbxMaterial);
        
        FbxDouble3 Specular = Phong->Specular.Get();
        MaterialInfo.SpecularColor = FVector(
            static_cast<float>(Specular[0]), 
            static_cast<float>(Specular[1]), 
            static_cast<float>(Specular[2])
        );
        
        MaterialInfo.Shininess = static_cast<float>(Phong->Shininess.Get());
        
        MaterialInfo.IOR = static_cast<float>(Phong->ReflectionFactor.Get());

        // Phong to MetallicRoughness
        // from Unreal Engine MF_PhongToMetalRoughness
        MaterialInfo.Metallic = (MaterialInfo.AmbientColor / 3.f).X;
        MaterialInfo.Roughness = FMath::Clamp(FMath::Pow(2.f / (FMath::Clamp(MaterialInfo.Shininess, 2.f, 1000.f) + 2.f), 0.25f), 0.f, 1.f);
    }

    ExtractTextureInfoFromFbx(FbxMaterial, MaterialInfo);

    return MaterialInfo;
}

void FFbxLoader::ExtractTextureInfoFromFbx(FbxSurfaceMaterial* FbxMaterial, FObjMaterialInfo& OutMaterialInfo)
{
    if (!FbxMaterial)
    {
        return;
    }

    const char* TextureTypes[] = {
        FbxSurfaceMaterial::sDiffuse,
        FbxSurfaceMaterial::sSpecular,
        FbxSurfaceMaterial::sNormalMap,
        FbxSurfaceMaterial::sEmissive,
        FbxSurfaceMaterial::sTransparencyFactor,
        FbxSurfaceMaterial::sAmbient,
        FbxSurfaceMaterial::sShininess
    };

    for (int i = 0; i < sizeof(TextureTypes) / sizeof(const char*); i++)
    {
        FbxProperty Property = FbxMaterial->FindProperty(TextureTypes[i]);
        if (Property.IsValid())
        {
            int TextureCount = Property.GetSrcObjectCount<FbxTexture>();
            for (int j = 0; j < TextureCount; j++)
            {
                FbxTexture* Texture = Property.GetSrcObject<FbxTexture>(j);
                if (Texture)
                {
                    FbxFileTexture* FileTexture = FbxCast<FbxFileTexture>(Texture);
                    if (FileTexture)
                    {
                        FTextureInfo TexInfo;
                        TexInfo.TextureName = FileTexture->GetName();
                        TexInfo.TexturePath = FString(FileTexture->GetFileName()).ToWideString();
                        
                        TexInfo.bIsSRGB = (i == 0 || i == 1 || i == 3 || i == 5);
                        
                        OutMaterialInfo.TextureInfos.Add(TexInfo);
                        
                        // 텍스처 플래그 설정
                        OutMaterialInfo.TextureFlag |= (1 << i); // 해당 텍스처 타입 플래그 설정
                    }
                }
            }
        }
    }
}

void FFbxLoader::ProcessSkeletonHierarchy(FbxNode* RootNode, FFbxLoadResult& OutResult)
{
    // 스켈레톤 계층 구조를 찾기 위한 첫 번째 패스
    TArray<FbxNode*> SkeletonRoots;
    FindSkeletonRootNodes(RootNode, SkeletonRoots);

    if (SkeletonRoots.IsEmpty())
    {
        return;
    }
    
    // 각 스켈레톤 루트 노드에 대해 전체 스켈레톤 생성
    for (FbxNode* SkeletonRoot : SkeletonRoots)
    {
        FbxPose* BindPose = FindBindPose(SkeletonRoot);
        
        USkeleton* NewSkeleton = FObjectFactory::ConstructObject<USkeleton>(nullptr);
        OutResult.Skeletons.Add(NewSkeleton);

        // 스켈레톤 구조 구축
        BuildSkeletonHierarchy(SkeletonRoot, NewSkeleton, BindPose);
    }
}

FbxPose* FFbxLoader::FindBindPose(FbxNode* SkeletonRoot)
{
    if (!Scene || !SkeletonRoot)
    {
        return nullptr;
    }

    // 스켈레톤에 속한 모든 본 노드를 수집
    TArray<FbxNode*> SkeletonBones;
    CollectSkeletonBoneNodes(SkeletonRoot, SkeletonBones);
    
    const int32 PoseCount = Scene->GetPoseCount();
    for (int32 PoseIndex = 0; PoseIndex < PoseCount; PoseIndex++)
    {
        FbxPose* CurrentPose = Scene->GetPose(PoseIndex);
        if (!CurrentPose || !CurrentPose->IsBindPose())
        {
            continue;
        }
            
        // 이 바인드 포즈가 스켈레톤의 일부 본을 포함하는지 확인
        bool bPoseContainsSomeBones = false;
        int32 NodeCount = CurrentPose->GetCount();
        
        for (int32 NodeIndex = 0; NodeIndex < NodeCount; NodeIndex++)
        {
            FbxNode* Node = CurrentPose->GetNode(NodeIndex);
            if (SkeletonBones.Contains(Node))
            {
                bPoseContainsSomeBones = true;
                break;
            }
        }
        
        // 이 스켈레톤에 바인드 포즈가 적어도 하나의 본을 포함하면 반환
        if (bPoseContainsSomeBones)
        {
            return CurrentPose;
        }
    }
    
    return nullptr; // 해당 스켈레톤에 관련된 바인드 포즈 없음
}

void FFbxLoader::CollectSkeletonBoneNodes(FbxNode* Node, TArray<FbxNode*>& OutBoneNodes)
{
    if (!Node)
    {
        return;
    }
    
    // 본 노드인지 확인
    if (Node->GetNodeAttribute() && 
        Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        OutBoneNodes.Add(Node);
    }
    
    // 자식 노드들에 대해 재귀적으로 처리
    for (int32 ChildIndex = 0; ChildIndex < Node->GetChildCount(); ChildIndex++)
    {
        FbxNode* ChildNode = Node->GetChild(ChildIndex);
        CollectSkeletonBoneNodes(ChildNode, OutBoneNodes);
    }
}

void FFbxLoader::FindSkeletonRootNodes(FbxNode* Node, TArray<FbxNode*>& OutSkeletonRoots)
{
    if (IsSkeletonRootNode(Node))
    {
        OutSkeletonRoots.Add(Node);
        return; // 이미 루트로 식별된 노드 아래는 더 탐색하지 않음
    }
    
    // 자식 노드들 재귀적으로 탐색
    for (int i = 0; i < Node->GetChildCount(); i++)
    {
        FindSkeletonRootNodes(Node->GetChild(i), OutSkeletonRoots);
    }
}

bool FFbxLoader::IsSkeletonRootNode(FbxNode* Node)
{
    if (!Node)
    {
        return false;
    }
    
    FbxNodeAttribute* Attribute = Node->GetNodeAttribute();
    if (Attribute && Attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        // 부모가 없거나 부모가 스켈레톤이 아닌 경우에만 루트로 간주
        FbxNode* Parent = Node->GetParent();
        if (Parent == nullptr || Parent->GetNodeAttribute() == nullptr || 
            Parent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton)
        {
            return true;
        }
    }
    return false;
}

void FFbxLoader::BuildSkeletonHierarchy(FbxNode* SkeletonRoot, USkeleton* OutSkeleton, FbxPose* BindPose)
{
    FReferenceSkeleton ReferenceSkeleton;
    
    CollectBoneData(SkeletonRoot, ReferenceSkeleton, INDEX_NONE, BindPose);

    OutSkeleton->SetReferenceSkeleton(ReferenceSkeleton);
}

void FFbxLoader::CollectBoneData(FbxNode* Node, FReferenceSkeleton& OutReferenceSkeleton, int32 ParentIndex, FbxPose* BindPose)
{
    if (!Node)
    {
        return;
    }
    
    TArray<FMeshBoneInfo>& RefBoneInfo = OutReferenceSkeleton.RawRefBoneInfo;
    TArray<FTransform>& RefBonePose = OutReferenceSkeleton.RawRefBonePose;
    TMap<FName, int32>& NameToIndexMap = OutReferenceSkeleton.RawNameToIndexMap;
    TArray<FMatrix>& InverseBindPoseMatrices = OutReferenceSkeleton.InverseBindPoseMatrices;
    
    FName BoneName = FName(Node->GetName());
    const int32 CurrentIndex = RefBoneInfo.Num();
    NameToIndexMap.Add(BoneName, CurrentIndex);
    
    // 뼈 정보 추가
    FMeshBoneInfo BoneInfo(BoneName, ParentIndex);
    RefBoneInfo.Add(BoneInfo);

    // 레퍼런스 포즈
    FTransform BoneTransform;
    int32 PoseNodeIndex = INDEX_NONE;
    if (BindPose)
    {
        PoseNodeIndex = BindPose->Find(Node);
    }
    if (PoseNodeIndex != INDEX_NONE)
    {
        // 현재 노드의 글로벌 바인드 포즈 행렬 가져오기
        const FbxMatrix& NodeMatrix = BindPose->GetMatrix(PoseNodeIndex);
        FbxAMatrix NodeGlobalMatrix;
    
        // FbxMatrix를 FbxAMatrix로 변환
        for (int32 r = 0; r < 4; ++r)
        {
            for (int32 c = 0; c < 4; ++c)
            {
                NodeGlobalMatrix[r][c] = NodeMatrix.Get(r, c);
            }
        }
    
        // 로컬 트랜스폼 계산
        FbxAMatrix LocalMatrix;
    
        if (ParentIndex != INDEX_NONE)
        {
            // 부모 노드 찾기
            FbxNode* ParentNode = Node->GetParent();
            if (ParentNode)
            {
                // 부모 노드의 바인드 포즈 인덱스 찾기
                int32 ParentPoseIndex = BindPose->Find(ParentNode);
            
                if (ParentPoseIndex != INDEX_NONE)
                {
                    // 부모 노드의 글로벌 바인드 포즈 행렬 가져오기
                    FbxMatrix ParentNodeMatrix = BindPose->GetMatrix(ParentPoseIndex);
                    FbxAMatrix ParentGlobalMatrix;
                
                    // FbxMatrix를 FbxAMatrix로 변환
                    for (int r = 0; r < 4; ++r)
                    {
                        for (int c = 0; c < 4; ++c)
                        {
                            ParentGlobalMatrix[r][c] = ParentNodeMatrix.Get(r, c);
                        }
                    }
                
                    // 로컬 트랜스폼 계산: Local = Global * ParentGlobal^-1
                    LocalMatrix = NodeGlobalMatrix * ParentGlobalMatrix.Inverse();
                }
                else
                {
                    // 부모의 바인드 포즈가 없으면 글로벌을 사용
                    LocalMatrix = NodeGlobalMatrix;
                }
            }
            else
            {
                // 부모 노드가 없으면 글로벌 사용
                LocalMatrix = NodeGlobalMatrix;
            }
        }
        else
        {
            // 루트 노드는 글로벌 = 로컬
            LocalMatrix = NodeGlobalMatrix;
        }
    
        // FbxAMatrix를 FTransform으로 변환
        BoneTransform = FTransform(ConvertFbxMatrixToFMatrix(LocalMatrix));
    }
    else
    {
        // 현재 노드 변환 사용
        BoneTransform = ConvertFbxTransformToFTransform(Node);
    }
    RefBonePose.Add(BoneTransform);
    
    // 역 바인드 포즈
    FbxAMatrix GlobalBindPoseMatrix;
    if (PoseNodeIndex != INDEX_NONE)
    {
        FbxMatrix Matrix = BindPose->GetMatrix(PoseNodeIndex);

        // FbxAMatrix로 요소 복사 (직접 캐스팅보다 안전)
        for (int r = 0; r < 4; ++r)
        {
            for (int c = 0; c < 4; ++c)
            {
                GlobalBindPoseMatrix[r][c] = Matrix.Get(r, c);
            }
        }
    }
    else
    {
        GlobalBindPoseMatrix.SetIdentity();
    }
    FbxAMatrix InverseBindMatrix = GlobalBindPoseMatrix.Inverse();
    FMatrix InverseBindPoseMatrix = ConvertFbxMatrixToFMatrix(InverseBindMatrix);
    InverseBindPoseMatrices.Add(InverseBindPoseMatrix);
    
    // 자식 노드들을 재귀적으로 처리
    for (int i = 0; i < Node->GetChildCount(); i++)
    {
        FbxNode* ChildNode = Node->GetChild(i);
        if (ChildNode &&
            ChildNode->GetNodeAttribute() &&
            ChildNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            CollectBoneData(ChildNode, OutReferenceSkeleton, CurrentIndex, BindPose);
        }
    }
}

FTransform FFbxLoader::ConvertFbxTransformToFTransform(FbxNode* Node) const
{
    FbxAMatrix LocalMatrix = Node->EvaluateLocalTransform();

    // FBX 행렬에서 스케일, 회전, 위치 추출
    FbxVector4 T = LocalMatrix.GetT();
    FbxVector4 S = LocalMatrix.GetS();
    FbxQuaternion Q = LocalMatrix.GetQ();
    
    // 언리얼 엔진 형식으로 변환
    FVector Translation(
        static_cast<float>(T[0]),
        static_cast<float>(T[1]),
        static_cast<float>(T[2])
    );
    
    FVector Scale(
        static_cast<float>(S[0]),
        static_cast<float>(S[1]),
        static_cast<float>(S[2])
    );
    
    FQuat Rotation(
        static_cast<float>(Q[0]),
        static_cast<float>(Q[1]),
        static_cast<float>(Q[2]),
        static_cast<float>(Q[3])
    );
    
    return FTransform(Rotation, Translation, Scale);
}

void FFbxLoader::ProcessMeshesWithSkeletons(FbxNode* Node, FFbxLoadResult& OutResult)
{
    if (Node && Node->GetNodeAttribute() && 
        Node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        FbxMesh* Mesh = Node->GetMesh();
        if (!Mesh)
        {
            return;
        }
        
        // 먼저 스킨 데이터가 있는지 확인하여 메시 유형 결정
        bool bHasSkin = false;
        for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(); ++DeformerIdx)
        {
            FbxDeformer* Deformer = Mesh->GetDeformer(DeformerIdx);
            if (Deformer && Deformer->GetDeformerType() == FbxDeformer::eSkin)
            {
                bHasSkin = true;
                break;
            }
        }
        
        if (bHasSkin)
        {
            // 이 메시와 연결된 스켈레톤 찾기
            USkeleton* AssociatedSkeleton = FindAssociatedSkeleton(Node, OutResult.Skeletons);
            
            if (AssociatedSkeleton)
            {
                // 스켈레탈 메시 생성 (스켈레톤 정보 전달)
                USkeletalMesh* SkeletalMesh = CreateSkeletalMeshFromNode(Node, AssociatedSkeleton);
                if (SkeletalMesh)
                {
                    SkeletalMesh->SetSkeleton(AssociatedSkeleton);
                    OutResult.SkeletalMeshes.Add(SkeletalMesh);
                }
            }
        }
        else
        {
            // 스태틱 메시 처리 (기존 TraverseNodeForMesh 함수로 처리)
            // 여기서는 생략
        }
    }
    
    // 자식 노드 재귀 처리
    for (int i = 0; i < Node->GetChildCount(); i++)
    {
        ProcessMeshesWithSkeletons(Node->GetChild(i), OutResult);
    }
}

USkeletalMesh* FFbxLoader::CreateSkeletalMeshFromNode(FbxNode* Node, USkeleton* Skeleton)
{
    FbxMesh* Mesh = Node->GetMesh();
    if (!Mesh)
    {
        return nullptr;
    }

    // 레이어 요소 가져오기 (UV, Normal, Tangent, Color 등은 레이어에 저장됨)
    // 보통 Layer 0을 사용
    FbxLayer* BaseLayer = Mesh->GetLayer(0);
    if (!BaseLayer)
    {
        OutputDebugStringA("Error: Mesh has no Layer 0.\n");
        return nullptr;
    }

    std::unique_ptr<FSkeletalMeshRenderData> RenderData = std::make_unique<FSkeletalMeshRenderData>();
    RenderData->DisplayName = Node->GetName();
    
    const FbxAMatrix LocalTransformMatrix = Node->EvaluateLocalTransform();

    // 정점 데이터 추출 및 병합
    const int32 PolygonCount = Mesh->GetPolygonCount(); // 삼각형 개수 (Triangulate 후)
    const FbxVector4* ControlPoints = Mesh->GetControlPoints(); // 제어점 (정점 위치) 배열
    const int32 ControlPointsCount = Mesh->GetControlPointsCount();

    // 정점 병합을 위한 맵
    TMap<FVertexKey, uint32> UniqueVertices;
    RenderData->Vertices.Reserve(ControlPointsCount); // 대략적인 크기 예약 (정확하지 않음)
    RenderData->Indices.Reserve(PolygonCount * 3);

    const FbxLayerElementNormal* NormalElement = BaseLayer->GetNormals();
    const FbxLayerElementTangent* TangentElement = BaseLayer->GetTangents();
    const FbxLayerElementUV* UVElement = BaseLayer->GetUVs();
    const FbxLayerElementVertexColor* ColorElement = BaseLayer->GetVertexColors();

    // 컨트롤 포인트별 본·스킨 가중치 맵
    TMap<int32, TArray<TPair<int32, double>>> SkinWeightMap;
    for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(FbxDeformer::eSkin); ++DeformerIdx)
    {
        FbxSkin* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(DeformerIdx, FbxDeformer::eSkin));
        for (int32 ClusterIdx = 0; ClusterIdx < Skin->GetClusterCount(); ++ClusterIdx)
        {
            FbxCluster* Cluster = Skin->GetCluster(ClusterIdx);
            FbxNode* LinkNode = Cluster->GetLink();
            if (!LinkNode)
            {
                continue;
            }
            
            int32 BoneIndex = -1;
            if (Skeleton)
            {
                BoneIndex = Skeleton->FindBoneIndex(LinkNode->GetName());
            }
            if (BoneIndex < 0)
            {
                continue;
            }
            
            int ControlPointCount = Cluster->GetControlPointIndicesCount();
            auto* ControlPointIndices = Cluster->GetControlPointIndices();
            auto* ControlPointWeights = Cluster->GetControlPointWeights();
        
            for (int ControlPointIdx = 0; ControlPointIdx < ControlPointCount; ++ControlPointIdx)
            {
                int32 ControlPoint = ControlPointIndices[ControlPointIdx];
                double Weight = ControlPointWeights[ControlPointIdx];
            
                if (Weight > 0.0)
                {
                    SkinWeightMap.FindOrAdd(ControlPoint).Add(TPair(BoneIndex, Weight));
                }
            }
        }
    }

    int VertexCounter = 0; // 폴리곤 정점 인덱스 (eByPolygonVertex 모드용)

    // 폴리곤(삼각형) 순회
    for (int32 i = 0; i < PolygonCount; ++i)
    {
        // 각 폴리곤(삼각형)의 정점 3개 순회
        for (int32 j = 0; j < 3; ++j)
        {
            const int32 ControlPointIndex = Mesh->GetPolygonVertex(i, j);

            FbxVector4 Position = ControlPoints[ControlPointIndex];
            FbxVector4 Normal;
            FbxVector4 Tangent;
            FbxVector2 UV;
            FbxColor Color;
            
            int NormalIndex = (NormalElement) ? (NormalElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
            int TangentIndex = (TangentElement) ? (TangentElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
            int UVIndex = (UVElement) ? (UVElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex ? Mesh->GetTextureUVIndex(i, j) : ControlPointIndex) : -1;
            int ColorIndex = (ColorElement) ? (ColorElement->GetMappingMode() == FbxLayerElement::eByControlPoint ? ControlPointIndex : VertexCounter) : -1;
            
            // 정점 병합 키 생성
            FVertexKey Key(ControlPointIndex, NormalIndex, TangentIndex, UVIndex, ColorIndex);

            // 맵에서 키 검색
            if (const uint32* Found = UniqueVertices.Find(Key))
            {
                RenderData->Indices.Add(*Found);
            }
            else
            {
                FSkeletalMeshVertex NewVertex;

                // Position
                if (ControlPointIndex < ControlPointsCount)
                {
                    Position = LocalTransformMatrix.MultT(Position);
                    SetVertexPosition(NewVertex, Position);
                }

                // Normal
                if (NormalElement && GetVertexElementData(NormalElement, ControlPointIndex, VertexCounter, Normal))
                {
                    Normal = LocalTransformMatrix.Inverse().Transpose().MultT(Normal);
                    SetVertexNormal(NewVertex, Normal);
                }

                // Tangent
                if (TangentElement && GetVertexElementData(TangentElement, ControlPointIndex, VertexCounter, Tangent))
                {
                     SetVertexTangent(NewVertex, Tangent);
                }

                // UV
                if(UVElement && GetVertexElementData(UVElement, ControlPointIndex, VertexCounter, UV))
                {
                    SetVertexUV(NewVertex, UV);
                }

                // Vertex Color
                if (ColorElement && GetVertexElementData(ColorElement, ControlPointIndex, VertexCounter, Color))
                {
                     SetVertexColor(NewVertex, Color);
                }

                // 본 데이터 설정
                auto& InfluenceList = SkinWeightMap[ControlPointIndex];
                std::sort(InfluenceList.begin(), InfluenceList.end(),
                    [](auto const& A, auto const& B)
                    {
                        return A.Value > B.Value; // Weight 기준 내림차순 정렬
                    }
                );
                
                double TotalWeight = 0.0;
                for (int32 BoneIdx = 0; BoneIdx < 4 && BoneIdx < InfluenceList.Num(); ++BoneIdx)
                {
                    NewVertex.BoneIndices[BoneIdx] = InfluenceList[BoneIdx].Key;
                    NewVertex.BoneWeights[BoneIdx] = static_cast<float>(InfluenceList[BoneIdx].Value);
                    TotalWeight += InfluenceList[BoneIdx].Value;
                }
                if (TotalWeight > 0.0)
                {
                    for (int BoneIdx = 0; BoneIdx < 4; ++BoneIdx)
                    {
                        NewVertex.BoneWeights[BoneIdx] /= static_cast<float>(TotalWeight);
                    }
                }

                // 새로운 정점을 Vertices 배열에 추가
                RenderData->Vertices.Add(NewVertex);
                // 새 정점의 인덱스 계산
                uint32 NewIndex = static_cast<uint32>(RenderData->Vertices.Num() - 1);
                // 인덱스 버퍼에 새 인덱스 추가
                RenderData->Indices.Add(NewIndex);
                // 맵에 새 정점 정보 추가
                UniqueVertices.Add(Key, NewIndex);
            }

            VertexCounter++; // 다음 폴리곤 정점으로 이동
        } // End for each vertex in polygon
    } // End for each polygon
    
    USkeletalMesh* SkeletalMesh = FObjectFactory::ConstructObject<USkeletalMesh>(nullptr);
    SkeletalMesh->SetRenderData(std::move(RenderData));

    return SkeletalMesh;
}

USkeleton* FFbxLoader::FindAssociatedSkeleton(FbxNode* MeshNode, const TArray<USkeleton*>& Skeletons)
{
    if (!MeshNode || Skeletons.Num() == 0)
    {
        return nullptr;
    }
    
    FbxMesh* Mesh = MeshNode->GetMesh();
    if (!Mesh)
    {
        return nullptr;
    }
    
    // 스킨 데이터가 있는지 확인
    bool bHasSkin = false;
    TSet<FbxNode*> BoneNodes;
    
    // 모든 스킨 디포머 순회
    for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(FbxDeformer::eSkin); ++DeformerIdx)
    {
        FbxSkin* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(DeformerIdx, FbxDeformer::eSkin));
        if (!Skin)
        {
            continue;
        }
        
        bHasSkin = true;
        
        // 모든 클러스터 순회하여 본 노드 수집
        for (int32 ClusterIdx = 0; ClusterIdx < Skin->GetClusterCount(); ++ClusterIdx)
        {
            FbxCluster* Cluster = Skin->GetCluster(ClusterIdx);
            if (Cluster && Cluster->GetLink())
            {
                BoneNodes.Add(Cluster->GetLink());
            }
        }
    }
    
    if (!bHasSkin || BoneNodes.Num() == 0)
    {
        return nullptr; // 스킨 데이터가 없으면 스태틱 메시로 간주
    }
    
    // 가장 많은 본을 공유하는 스켈레톤 찾기
    USkeleton* BestMatch = nullptr;
    int32 MaxSharedBones = 0;
    
    for (USkeleton* Skeleton : Skeletons)
    {
        int32 SharedBones = 0;
        
        // 현재 스켈레톤의 모든 본 이름 확인
        const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
        for (FbxNode* BoneNode : BoneNodes)
        {
            FName BoneName(BoneNode->GetName());
            if (RefSkeleton.FindBoneIndex(BoneName) != INDEX_NONE)
            {
                SharedBones++;
            }
        }
        
        if (SharedBones > MaxSharedBones)
        {
            MaxSharedBones = SharedBones;
            BestMatch = Skeleton;
        }
    }
    
    return BestMatch;
}

void FFbxLoader::ExtractBindPoseMatrices(const FbxMesh* Mesh, const USkeleton* Skeleton, TArray<FMatrix>& OutInverseBindPoseMatrices) const
{
    if (!Mesh || !Skeleton)
    {
        return;
    }
    
    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
    const int32 BoneCount = RefSkeleton.RawRefBoneInfo.Num();
    
    // 역행렬 배열 초기화 (단위 행렬로)
    OutInverseBindPoseMatrices.SetNum(BoneCount);
    for (int32 i = 0; i < BoneCount; ++i)
    {
        OutInverseBindPoseMatrices[i] = FMatrix::Identity;
    }
    
    // 모든 스킨 디포머 순회
    for (int32 DeformerIdx = 0; DeformerIdx < Mesh->GetDeformerCount(FbxDeformer::eSkin); ++DeformerIdx)
    {
        FbxSkin* Skin = static_cast<FbxSkin*>(Mesh->GetDeformer(DeformerIdx, FbxDeformer::eSkin));
        if (!Skin)
        {
            continue;
        }
        
        // 모든 클러스터(본) 순회
        for (int32 ClusterIdx = 0; ClusterIdx < Skin->GetClusterCount(); ++ClusterIdx)
        {
            FbxCluster* Cluster = Skin->GetCluster(ClusterIdx);
            if (!Cluster || !Cluster->GetLink())
            {
                continue;
            }
            
            // 본 인덱스 찾기
            FName BoneName(Cluster->GetLink()->GetName());
            int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
            if (BoneIndex == INDEX_NONE)
            {
                continue;
            }
            
            // 바인드 포즈 행렬 가져오기
            FbxAMatrix TransformMatrix;      // 메시의 월드 변환
            FbxAMatrix TransformLinkMatrix;  // 본의 월드 변환
            Cluster->GetTransformMatrix(TransformMatrix);
            Cluster->GetTransformLinkMatrix(TransformLinkMatrix);
            
            // 스키닝 행렬 계산 (오프셋 행렬)
            // 메시 로컬 공간의 정점을 본 로컬 공간으로 변환하는 행렬
            FbxAMatrix OffsetMatrix = TransformLinkMatrix.Inverse() * TransformMatrix;
            
            // FBX 행렬을 FMatrix로 변환하여 저장
            OutInverseBindPoseMatrices[BoneIndex] = ConvertFbxMatrixToFMatrix(OffsetMatrix);
        }
    }
}

FMatrix FFbxLoader::ConvertFbxMatrixToFMatrix(const FbxAMatrix& FbxMatrix) const
{
    FMatrix Result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            Result.M[i][j] = static_cast<float>(FbxMatrix.Get(i, j));
        }
    }
    return Result;

}

void FFbxLoader::ConvertSceneToLeftHandedZUpXForward(FbxScene* Scene)
{
    if (!Scene)
    {
        return;
    }
    
    // 현재 장면의 좌표계를 가져옴
    FbxAxisSystem SceneAxisSystem = Scene->GetGlobalSettings().GetAxisSystem();
    
    // 왼손 좌표계, Z-up, X-forward 좌표계 정의
    // X-forward는 ParityOdd와 함께 설정하여 X축이 앞쪽을 향하게 함
    FbxAxisSystem TargetAxisSystem(FbxAxisSystem::eZAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eLeftHanded);
    
    // 현재 좌표계와 목표 좌표계가 다른 경우에만 변환
    if (SceneAxisSystem != TargetAxisSystem)
    {
        OutputDebugStringA("Converting coordinate system to Left-Handed Z-Up X-Forward\n");
        TargetAxisSystem.DeepConvertScene(Scene);
    }
    else
    {
        OutputDebugStringA("Scene already uses the target coordinate system\n");
    }
}
