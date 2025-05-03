
#include "SkeletalMesh.h"
#include "Asset/SkeletalMeshAsset.h"

USkeletalMesh::~USkeletalMesh()
{
    OutputDebugStringA("USkeletalMesh destroyed");
}

void USkeletalMesh::SetData(std::unique_ptr<FSkeletalMeshRenderData> InRenderData)
{
    RenderData = std::move(InRenderData);
}
