
#include "FbxLoader.h"

FFbxLoader::FFbxLoader()
    : Manager(nullptr)
    , Importer(nullptr)
    , Scene(nullptr)
{
    Manager = FbxManager::Create();

    FbxIOSettings* IOSettings = FbxIOSettings::Create(Manager, IOSROOT);
    Manager->SetIOSettings(IOSettings);

    IOSettings->SetBoolProp(IMP_FBX_MATERIAL, false);
    IOSettings->SetBoolProp(IMP_FBX_TEXTURE, false);
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

bool FFbxLoader::LoadFBX(const FString& InFilePath)
{
    bool bRet = false;
    if (Importer->Initialize(*InFilePath, -1, Manager->GetIOSettings()))
    {
        bRet = Importer->Import(Scene);
    }

    if (bRet)
    {
        // FbxAxisSystem::Max.ConvertScene(Scene); // 언리얼 엔진 방식 좌표축
        FbxNode* RootNode = Scene->GetRootNode();
        for (int32 i = 0; i < RootNode->GetChildCount(); ++i)
        {
            FbxNode* ChildNode = RootNode->GetChild(i);
            
        }
    }
    return bRet;
}

USkeletalMesh* FFbxManager::CreateMesh(const FString& MeshPath)
{
    /*
    FSkeletalMeshRenderData* StaticMeshRenderData = FGbxManager::LoadObjStaticMeshAsset(filePath);

    if (StaticMeshRenderData == nullptr) return nullptr;

    UStaticMesh* StaticMesh = GetStaticMesh(StaticMeshRenderData->ObjectName);
    if (StaticMesh != nullptr)
    {
        return StaticMesh;
    }

    StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>(nullptr); // TODO: 추후 AssetManager를 생성해서 관리.
    StaticMesh->SetData(StaticMeshRenderData);

    StaticMeshMap.Add(StaticMeshRenderData->ObjectName, StaticMesh); // TODO: 장기적으로 보면 파일 이름 대신 경로를 Key로 사용하는게 좋음.
    return StaticMesh;
    */
    return nullptr;
}
