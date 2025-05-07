#pragma once

#include <fbxsdk.h>

#include "HAL/PlatformType.h"
#include "Container/Array.h"
#include "Container/Map.h"

struct FObjMaterialInfo;
struct FReferenceSkeleton;
struct FTransform;
struct FMeshBoneInfo;
class USkeleton;
class FString;
class USkeletalMesh;
struct FSkeletalMeshRenderData;
struct FFbxLoadResult;
struct FMatrix;

class FFbxLoader
{
public:
    FFbxLoader();
    ~FFbxLoader();

    FFbxLoadResult LoadFBX(const FString& InFilePath);

private:
    FbxManager* Manager;
    FbxImporter* Importer;
    FbxScene* Scene;

    FWString FilePath;
    FWString ObjectName;
    FString DisplayName;

    // Begin Material
    void ProcessMaterials(FFbxLoadResult& OutResult);

    FObjMaterialInfo ExtractMaterialsFromFbx(FbxSurfaceMaterial* FbxMaterial);

    void ExtractTextureInfoFromFbx(FbxSurfaceMaterial* FbxMaterial, FObjMaterialInfo& OutMaterialInfo);
    // End Material

    // Begin Skeleton
    void ProcessSkeletonHierarchy(FbxNode* RootNode, FFbxLoadResult& OutResult);

    FbxPose* FindBindPose(FbxNode* SkeletonRoot);

    void CollectSkeletonBoneNodes(FbxNode* Node, TArray<FbxNode*>& OutBoneNodes);
    
    void FindSkeletonRootNodes(FbxNode* Node, TArray<FbxNode*>& OutSkeletonRoots);

    bool IsSkeletonRootNode(FbxNode* Node);

    void BuildSkeletonHierarchy(FbxNode* SkeletonRoot, USkeleton* OutSkeleton, FbxPose* BindPose);

    void CollectBoneData(FbxNode* Node, FReferenceSkeleton& OutReferenceSkeleton, int32 ParentIndex, FbxPose* BindPose);

    FTransform ConvertFbxTransformToFTransform(FbxNode* Node) const;
    // End Skeleton
    
    // Begin Mesh
    void ProcessMeshes(FbxNode* Node, FFbxLoadResult& OutResult);

    void CollectMeshNodes(FbxNode* Node, const TArray<USkeleton*>& Skeletons, TMap<USkeleton*, TArray<FbxNode*>>& OutSkeletalMeshNodes , TArray<FbxNode*>& OutStaticMeshNodes);

    USkeletalMesh* CreateSkeletalMeshFromNode(TArray<FbxNode*> MeshNodes, USkeleton* Skeleton, int32 GlobalMeshIdx);

    USkeleton* FindAssociatedSkeleton(FbxNode* Node, const TArray<USkeleton*>& Skeletons);

    void ExtractBindPoseMatrices(const FbxMesh* Mesh, const USkeleton* Skeleton, TArray<FMatrix>& OutInverseBindPoseMatrices) const;
    
    FMatrix ConvertFbxMatrixToFMatrix(const FbxAMatrix& FbxMatrix) const;
    // End Mesh

    // 좌표계 변환 메소드
    void ConvertSceneToLeftHandedZUpXForward(FbxScene* Scene);
};

class FFbxManager {
public:
    static USkeletalMesh* LastPickSkeletalMesh;
};
