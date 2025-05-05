#pragma once

#include <fbxsdk.h>

#include "HAL/PlatformType.h"
#include "Container/Array.h"

struct FReferenceSkeleton;
struct FTransform;
struct FMeshBoneInfo;
class USkeleton;
class FString;
class USkeletalMesh;
struct FSkeletalMeshRenderData;
struct FFbxLoadResult;

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

    // Begin Skeleton
    void ProcessSkeletonHierarchy(FbxNode* RootNode, FFbxLoadResult& OutResult);

    void FindSkeletonRootNodes(FbxNode* Node, TArray<FbxNode*>& OutSkeletonRoots);

    bool IsSkeletonRootNode(FbxNode* Node);

    void BuildSkeletonHierarchy(FbxNode* SkeletonRoot, USkeleton* OutSkeleton);

    void CollectBoneData(FbxNode* Node, FReferenceSkeleton& OutReferenceSkeleton, int32 ParentIndex);

    FTransform ConvertFbxTransformToFTransform(FbxNode* Node) const;
    // End Skeleton
    
    // Begin Mesh
    void ProcessMeshesWithSkeletons(FbxNode* Node, FFbxLoadResult& OutResult);

    USkeletalMesh* CreateSkeletalMeshFromNode(FbxNode* Node, USkeleton* Skeleton);

    USkeleton* FindAssociatedSkeleton(FbxNode* Node, const TArray<USkeleton*>& Skeletons);
    // End Mesh

    // 좌표계 변환을 수행하는 헬퍼 메소드
    void ConvertSceneToLeftHandedZUpXForward(FbxScene* Scene);
    
    // 좌표계 변환을 위한 변환 행렬 계산
    void CalculateCoordinateSystemTransform(const FbxAxisSystem& SourceAxisSystem);
};
