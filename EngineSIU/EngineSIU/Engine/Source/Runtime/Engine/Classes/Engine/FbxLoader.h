#pragma once

#include <fbxsdk.h>

#include "Container/String.h"
#include "SkeletalMesh.h"

class FFbxLoader
{
public:
    FFbxLoader();
    ~FFbxLoader();

    bool LoadFBX(const FString& InFilePath);

private:
    FbxManager* Manager;
    FbxImporter* Importer;
    FbxScene* Scene;
};

class FFbxManager
{
public:
    //static FSkeletalMeshRenderData* LoadFbxSkeletalMeshAsset(const FString& MeshPath);
    static USkeletalMesh* CreateMesh(const FString& MeshPath);
};
