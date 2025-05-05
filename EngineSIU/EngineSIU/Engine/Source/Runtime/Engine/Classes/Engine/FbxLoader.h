#pragma once

#include <fbxsdk.h>

#include "Container/Map.h"
#include "HAL/PlatformType.h"

class USkeletalMesh;
struct FSkeletalMeshRenderData;

class FFbxLoader
{
public:
    FFbxLoader();
    ~FFbxLoader();

    bool LoadFBX(const FString& InFilePath, FSkeletalMeshRenderData& OutRenderData);

private:
    FbxManager* Manager;
    FbxImporter* Importer;
    FbxScene* Scene;

    void TraverseNodeForSkeleton(FbxNode* Node, FSkeletalMeshRenderData& OutRenderData);

    void TraverseNodeForMesh(FbxNode* Node, FSkeletalMeshRenderData& OutRenderData);

    void ProcessSkeleton(FbxNode* Node, FSkeletalMeshRenderData& OutRenderData);

    void ProcessMesh(FbxNode* Node, FSkeletalMeshRenderData& OutRenderData);
};

class FFbxManager
{
public:
    static USkeletalMesh* GetSkeletalMesh(const FWString& FilePath);

protected:
    static std::unique_ptr<FSkeletalMeshRenderData> LoadFbxSkeletalMeshAsset(const FWString& FilePath);
    static USkeletalMesh* CreateMesh(const FWString& FilePath);

    inline static TMap<FString, USkeletalMesh*> SkeletalMeshMap;
};
