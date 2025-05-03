
#include "FbxLoader.h"

#include <format>

#include "Asset/SkeletalMeshAsset.h"
#include "UObject/ObjectFactory.h"
#include "SkeletalMesh.h"

struct FVertexKey
{
    int32 ControlPointIndex;
    int32 NormalIndex;
    int32 TangentIndex;
    int32 UVIndex;
    int32 ColorIndex;

    bool operator==(const FVertexKey& Other) const
    {
        return ControlPointIndex == Other.ControlPointIndex
            && NormalIndex == Other.NormalIndex
            && TangentIndex == Other.TangentIndex
            && UVIndex == Other.UVIndex
            && ColorIndex == Other.ColorIndex;
    }
};

namespace std
{
    template<>
    struct hash<FVertexKey>
    {
        size_t operator()(const FVertexKey& Key) const
        {
            size_t Seed = 0;
            hash_combine(Seed, Key.ControlPointIndex);
            hash_combine(Seed, Key.NormalIndex);
            hash_combine(Seed, Key.TangentIndex);
            hash_combine(Seed, Key.UVIndex);
            hash_combine(Seed, Key.ColorIndex);
            return Seed;
        }
        
        // hash_combine 함수 구현 필요
        void hash_combine(size_t& Seed, int32 Value) const
        {
            Seed ^= std::hash<int32>{}(Value) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
        }
    };
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 XYZ로 변환 (좌표계 변환 포함)
inline void SetVertexPosition(FSkeletalMeshVertex& Vertex, const FbxVector4& Pos)
{
    // FbxAxisSystem::Max.ConvertScene(Scene) 사용 시 (Max Z-up -> Unreal Z-up 가정)
    // X -> X, Y -> -Y, Z -> Z
    Vertex.X = static_cast<float>(Pos[0]);
    Vertex.Y = static_cast<float>(Pos[1]); // Y축 반전
    Vertex.Z = static_cast<float>(Pos[2]);
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 Normal XYZ로 변환 (좌표계 변환 포함)
inline void SetVertexNormal(FSkeletalMeshVertex& Vertex, const FbxVector4& Normal)
{
    // FbxAxisSystem::Max.ConvertScene(Scene) 사용 시
    Vertex.NormalX = static_cast<float>(Normal[0]);
    Vertex.NormalY = static_cast<float>(Normal[1]); // Y축 반전
    Vertex.NormalZ = static_cast<float>(Normal[2]);
}

// 헬퍼 함수: FbxVector4를 FSkeletalMeshVertex의 Tangent XYZW로 변환 (좌표계 변환 포함)
inline void SetVertexTangent(FSkeletalMeshVertex& Vertex, const FbxVector4& Tangent)
{
    // FbxAxisSystem::Max.ConvertScene(Scene) 사용 시
    Vertex.TangentX = static_cast<float>(Tangent[0]);
    Vertex.TangentY = static_cast<float>(Tangent[1]); // Y축 반전
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

    int32 Index = -1;
    switch (MappingMode)
    {
    case FbxLayerElement::eByControlPoint:
        Index = ControlPointIndex;
        break;
    case FbxLayerElement::eByPolygonVertex:
        Index = VertexIndex;
        break;
    // 다른 매핑 모드(eByPolygon, eByEdge, eAllSame)는 이 컨텍스트에서 덜 일반적임
    case FbxLayerElement::eAllSame: // 모든 정점이 같은 값을 가짐
         if (Element->GetDirectArray().GetCount() > 0)
         {
             OutData = Element->GetDirectArray().GetAt(0);
             return true;
         }
         break;
    default:
        break;
    }

    if (Index < Element->GetIndexArray().GetCount())
    {
        switch (ReferenceMode)
        {
        case FbxLayerElement::eDirect:
            OutData = Element->GetDirectArray().GetAt(Index);
            return true;
        case FbxLayerElement::eIndexToDirect:
        case FbxLayerElement::eIndex: // eIndex는 사용되지 않음
            {
                int32 DirectIndex = Element->GetIndexArray().GetAt(Index);
                if (DirectIndex < Element->GetDirectArray().GetCount())
                {
                    OutData = Element->GetDirectArray().GetAt(DirectIndex);
                    return true;
                }
            }
            break;
        default:
            break; // 다른 레퍼런스 모드는 지원하지 않음
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

bool FFbxLoader::LoadFBX(const FString& InFilePath, FSkeletalMeshRenderData& OutRenderData)
{
    bool bRet = false;
    if (Importer->Initialize(*InFilePath, -1, Manager->GetIOSettings()))
    {
        bRet = Importer->Import(Scene);
    }
    if (!bRet)
    {
        return false;
    }

    // Basic Setup
    OutRenderData.ObjectName = InFilePath.ToWideString();
    OutRenderData.DisplayName = ""; // TODO: temp

    // Read FBX
    int32 UpSign = 1;
    FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::eZAxis;

    int32 FrontSign = 1;
    FbxAxisSystem::EFrontVector FrontVector = FbxAxisSystem::eParityEven;

    FbxAxisSystem::ECoordSystem CoordSystem = FbxAxisSystem::eLeftHanded;

    FbxAxisSystem DesiredAxisSystem(UpVector, FrontVector, CoordSystem);
    
    // DesiredAxisSystem.ConvertScene(Scene); // 언리얼 엔진 방식 좌표축
    //FbxAxisSystem::Max.ConvertScene(Scene); // 언리얼 엔진 방식 좌표축

    const FbxGlobalSettings& GlobalSettings = Scene->GetGlobalSettings();
    FbxSystemUnit SystemUnit = GlobalSettings.GetSystemUnit();
    const double ScaleFactor = SystemUnit.GetScaleFactor();
    OutputDebugStringA(std::format("### FBX ###\nScene Scale: {} cm\n", ScaleFactor).c_str());
    
    if (FbxNode* RootNode = Scene->GetRootNode())
    {
        FbxGeometryConverter Converter(Manager);
        Converter.Triangulate(Scene, true);
        
        TraverseNodeRecursive(RootNode, OutRenderData);
    }
    
    return true;
}

void FFbxLoader::TraverseNodeRecursive(FbxNode* Node, FSkeletalMeshRenderData& OutRenderData)
{
    if (!Node)
    {
        return;
    }

    FbxNodeAttribute* Attribute = Node->GetNodeAttribute();
    if (Attribute)
    {
        switch (Attribute->GetAttributeType())
        {
        case FbxNodeAttribute::eMesh:
            ProcessMesh(Node, OutRenderData);
            break;
        case FbxNodeAttribute::eSkeleton:
            break;
        default:
            break;
        }
    }

    for (int32 i = 0; i < Node->GetChildCount(); ++i)
    {
        TraverseNodeRecursive(Node->GetChild(i), OutRenderData);
    }
}

void FFbxLoader::ProcessMesh(FbxNode* Node, FSkeletalMeshRenderData& OutRenderData)
{
    FbxMesh* Mesh = Node->GetMesh();
    if (!Mesh)
    {
        return;
    }

    // 이미 데이터가 있다면 병합하지 않고 반환 (여러 메쉬 노드를 어떻게 처리할지 정책 필요)
    // 여기서는 첫 번째 찾은 메쉬만 사용한다고 가정
    if (!OutRenderData.Vertices.IsEmpty())
    {
        OutputDebugStringA(std::format("Skipping additional mesh node: {}. Already processed one.\n", Node->GetName()).c_str());
        return;
    }

    FbxGeometryConverter Converter(Manager);
    FbxAMatrix Matrix = Node->EvaluateLocalTransform();

    // 정점 데이터 추출 및 병합
    const int32 PolygonCount = Mesh->GetPolygonCount(); // 삼각형 개수 (Triangulate 후)
    const FbxVector4* ControlPoints = Mesh->GetControlPoints(); // 제어점 (정점 위치) 배열
    const int32 ControlPointsCount = Mesh->GetControlPointsCount();

    // 정점 병합을 위한 맵
    TMap<FVertexKey, uint32> UniqueVertices;
    OutRenderData.Vertices.Reserve(ControlPointsCount); // 대략적인 크기 예약 (정확하지 않음)
    OutRenderData.Indices.Reserve(PolygonCount * 3);

    // 레이어 요소 가져오기 (UV, Normal, Tangent, Color 등은 레이어에 저장됨)
    // 보통 Layer 0을 사용
    FbxLayer* BaseLayer = Mesh->GetLayer(0);
    if (!BaseLayer)
    {
        OutputDebugStringA("Error: Mesh has no Layer 0.\n");
        return;
    }

    const FbxLayerElementNormal* NormalElement = BaseLayer->GetNormals();
    const FbxLayerElementTangent* TangentElement = BaseLayer->GetTangents();
    const FbxLayerElementUV* UVElement = BaseLayer->GetUVs(); // 기본 UV 세트 (이름으로 다른 세트 접근 가능)
    const FbxLayerElementVertexColor* ColorElement = BaseLayer->GetVertexColors();

    int VertexCounter = 0; // 폴리곤 정점 인덱스 (eByPolygonVertex 모드용)

    // 폴리곤(삼각형) 순회
    for (int32 i = 0; i < PolygonCount; ++i)
    {
        // 각 폴리곤(삼각형)의 정점 3개 순회
        for (int32 j = 0; j < 3; ++j) // Triangulate 했으므로 항상 3개
        {
            const int32 ControlPointIndex = Mesh->GetPolygonVertex(i, j);

            // --- 현재 정점의 속성 인덱스 결정 ---
            // 속성 데이터 자체를 키로 사용하면 복잡하고 느릴 수 있으므로,
            // 각 속성 배열에서의 인덱스를 조합하여 키로 사용합니다.
            // GetVertexElementData 헬퍼 함수를 사용하여 실제 데이터는 나중에 가져옵니다.
            // 이 방식은 동일한 (위치, 노멀, UV 등) 조합을 가진 정점을 병합합니다.

            // 임시 변수 (실제 데이터는 나중에 채움)
            FbxVector4 TempNormal;
            FbxVector4 TempTangent;
            FbxVector2 TempUV;
            FbxColor TempColor;

            // 각 속성별 인덱스 가져오기 시도 (GetVertexElementData 내부 로직과 유사하게 인덱스 결정)
            // 간단하게 하기 위해, 여기서는 ControlPointIndex 또는 VertexCounter를 기반으로
            // 해당 속성 데이터가 "어디서" 오는지만 식별하는 인덱스를 만듭니다.
            // 더 정확하게 하려면 GetVertexElementData 헬퍼처럼 실제 인덱스를 찾아야 합니다.

            int NormalIndex = -1; // 기본값 -1 (없음)
            if (NormalElement)
            {
                if (NormalElement->GetMappingMode() == FbxLayerElement::eByControlPoint)
                {
                    NormalIndex = (NormalElement->GetReferenceMode() == FbxLayerElement::eDirect) ? ControlPointIndex : Mesh->GetElementNormal()->GetIndexArray().GetAt(ControlPointIndex);
                }
                else if (NormalElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                {
                    NormalIndex = (NormalElement->GetReferenceMode() == FbxLayerElement::eDirect) ? VertexCounter : Mesh->GetElementNormal()->GetIndexArray().GetAt(VertexCounter);
                }
                 // else if (NormalElement->GetMappingMode() == FbxLayerElement::eAllSame) NormalIndex = 0;
            }


            int TangentIndex = -1;
            if (TangentElement)
            {
                 if (TangentElement->GetMappingMode() == FbxLayerElement::eByControlPoint)
                 {
                     TangentIndex = (TangentElement->GetReferenceMode() == FbxLayerElement::eDirect) ? ControlPointIndex : Mesh->GetElementTangent()->GetIndexArray().GetAt(ControlPointIndex);
                 }
                 else if (TangentElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                 {
                     TangentIndex = (TangentElement->GetReferenceMode() == FbxLayerElement::eDirect) ? VertexCounter : Mesh->GetElementTangent()->GetIndexArray().GetAt(VertexCounter);
                 }
                // else if (TangentElement->GetMappingMode() == FbxLayerElement::eAllSame) TangentIndex = 0;
            }

            int UVIndex = -1;
            if (UVElement)
            {
                if (UVElement->GetMappingMode() == FbxLayerElement::eByControlPoint)
                {
                    UVIndex = (UVElement->GetReferenceMode() == FbxLayerElement::eDirect) ? ControlPointIndex : Mesh->GetElementUV()->GetIndexArray().GetAt(ControlPointIndex);
                }
                else if (UVElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                {
                    // UV는 GetTextureUVIndex 사용 가능
                     UVIndex = Mesh->GetTextureUVIndex(i, j); // 이게 더 정확할 수 있음
                }
                // else if (UVElement->GetMappingMode() == FbxLayerElement::eAllSame) UVIndex = 0;
            }

            int ColorIndex = -1;
            if (ColorElement)
            {
                 if (ColorElement->GetMappingMode() == FbxLayerElement::eByControlPoint)
                 {
                     ColorIndex = (ColorElement->GetReferenceMode() == FbxLayerElement::eDirect) ? ControlPointIndex : Mesh->GetElementVertexColor()->GetIndexArray().GetAt(ControlPointIndex);
                 }
                 else if (ColorElement->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
                 {
                     ColorIndex = (ColorElement->GetReferenceMode() == FbxLayerElement::eDirect) ? VertexCounter : Mesh->GetElementVertexColor()->GetIndexArray().GetAt(VertexCounter);
                 }
                 // else if (ColorElement->GetMappingMode() == FbxLayerElement::eAllSame) ColorIndex = 0;
            }

            // 정점 병합 키 생성
            FVertexKey Key(ControlPointIndex, NormalIndex, TangentIndex, UVIndex, ColorIndex);

            // 맵에서 키 검색
            if (const uint32* Iter = UniqueVertices.Find(Key))
            {
                // 이미 존재하는 정점: 인덱스 버퍼에 기존 인덱스 추가
                OutRenderData.Indices.Add(*Iter);
            }
            else
            {
                // 새로운 고유 정점:
                FSkeletalMeshVertex NewVertex;

                // 1. 위치 설정 (Control Point 사용)
                if (ControlPointIndex < ControlPointsCount)
                {
                    SetVertexPosition(NewVertex, Matrix.MultT(ControlPoints[ControlPointIndex]));
                }

                // 2. 노멀 설정 (GetVertexElementData 사용)
                if (NormalElement && GetVertexElementData(NormalElement, ControlPointIndex, VertexCounter, TempNormal))
                {
                    SetVertexNormal(NewVertex, Matrix.Inverse().Transpose().MultT(TempNormal));
                }
                else
                {
                    // 노멀 데이터 없을 경우 기본값 사용 (FSkeletalMeshVertex 생성자에서 설정됨)
                     OutputDebugStringA(std::format("Warning: Normal data not found for vertex (CP Index: {}, Vtx Counter: {}). Using default.\n", ControlPointIndex, VertexCounter).c_str());
                }

                // 3. 탄젠트 설정 (GetVertexElementData 사용)
                if (TangentElement && GetVertexElementData(TangentElement, ControlPointIndex, VertexCounter, TempTangent))
                {
                     SetVertexTangent(NewVertex, TempTangent);
                }
                else
                {
                    // 탄젠트 데이터 없을 경우 기본값 사용 (FSkeletalMeshVertex 생성자에서 설정됨)
                    // 또는 노멀과 기본 UV에서 계산할 수도 있음 (복잡)
                    // OutputDebugStringA(std::format("Warning: Tangent data not found for vertex (CP Index: {}, Vtx Counter: {}). Using default.\n", ControlPointIndex, VertexCounter).c_str());
                }

                // 4. UV 설정 (GetVertexElementData 사용, UV 인덱스는 GetTextureUVIndex 사용 권장)
                // UVElement를 직접 사용하거나, Mesh->GetPolygonVertexUV 사용 가능
                FbxStringList UVSetNameList;
                Mesh->GetUVSetNames(UVSetNameList);
                const char* UVSetName = UVSetNameList.GetCount() > 0 ? UVSetNameList.GetStringAt(0) : nullptr; // 첫 번째 UV 세트 이름 사용

                bool bUVFound = false;
                if (UVSetName && UVElement) // UV 세트 이름이 있고 UV 요소가 있을 때
                {
                    if(GetVertexElementData(UVElement, ControlPointIndex, VertexCounter, TempUV))
                    {
                         SetVertexUV(NewVertex, TempUV);
                         bUVFound = true;
                    }
                }

                if (!bUVFound)
                {
                     // UV 데이터 없을 경우 기본값 사용 (FSkeletalMeshVertex 생성자에서 설정됨)
                     // OutputDebugStringA(std::format("Warning: UV data not found for vertex (CP Index: {}, Vtx Counter: {}). Using default (0,0).\n", ControlPointIndex, VertexCounter).c_str());
                }


                // 5. 색상 설정 (GetVertexElementData 사용)
                if (ColorElement && GetVertexElementData(ColorElement, ControlPointIndex, VertexCounter, TempColor))
                {
                     SetVertexColor(NewVertex, TempColor);
                }
                else
                {
                    // 색상 데이터 없을 경우 기본값 사용 (FSkeletalMeshVertex 생성자에서 설정됨)
                }

                // 6. 본 데이터 설정 (현재는 기본값 사용)
                // 실제로는 FbxSkin, FbxCluster 등을 처리해야 함 (생략)
                NewVertex.BoneIndices[0] = 0;
                NewVertex.BoneWeights[0] = 1.0f;
                for(int k = 1; k < 4; ++k)
                {
                    NewVertex.BoneIndices[k] = 0;
                    NewVertex.BoneWeights[k] = 0.0f;
                }

                // 새로운 정점을 Vertices 배열에 추가
                OutRenderData.Vertices.Add(NewVertex);

                // 새 정점의 인덱스 계산
                uint32 NewIndex = static_cast<uint32>(OutRenderData.Vertices.Num() - 1);

                // 인덱스 버퍼에 새 인덱스 추가
                OutRenderData.Indices.Add(NewIndex);

                // 맵에 새 정점 정보 추가
                UniqueVertices[Key] = NewIndex;
            }

            VertexCounter++; // 다음 폴리곤 정점으로 이동
        } // End for each vertex in polygon
    } // End for each polygon

    // 정점 병합 후 결과 출력 (디버깅용)
    std::string msg = std::format("Mesh '{}' processed: {} unique vertices, {} indices (Original Control Points: {})\n",
                                   Node->GetName(),
                                   OutRenderData.Vertices.Num(),
                                   OutRenderData.Indices.Num(),
                                   ControlPointsCount);
    OutputDebugStringA(msg.c_str());
}

std::unique_ptr<FSkeletalMeshRenderData> FFbxManager::LoadFbxSkeletalMeshAsset(const FWString& FilePath)
{
    std::unique_ptr<FSkeletalMeshRenderData> Data = std::make_unique<FSkeletalMeshRenderData>();

    // TODO: 여기에서 바이너리 파일 검색해서 찾으면 읽고 Data 채워서 리턴.

    FFbxLoader Loader;
    if (Loader.LoadFBX(FilePath, *Data))
    {
        return Data;
    }
    return nullptr;
}

USkeletalMesh* FFbxManager::CreateMesh(const FWString& FilePath)
{
    std::unique_ptr<FSkeletalMeshRenderData> SkeletalMeshRenderData = LoadFbxSkeletalMeshAsset(FilePath);

    if (SkeletalMeshRenderData == nullptr)
    {
        return nullptr;
    }

    USkeletalMesh* Mesh = FObjectFactory::ConstructObject<USkeletalMesh>(nullptr);
    if (Mesh)
    {
        Mesh->SetData(std::move(SkeletalMeshRenderData));
        SkeletalMeshMap.Add(FilePath, Mesh);
    }
    
    return Mesh;
}

USkeletalMesh* FFbxManager::GetSkeletalMesh(const FWString& FilePath)
{
    if (SkeletalMeshMap.Find(FilePath))
    {
        return SkeletalMeshMap[FilePath];
    }
    return CreateMesh(FilePath);
}
