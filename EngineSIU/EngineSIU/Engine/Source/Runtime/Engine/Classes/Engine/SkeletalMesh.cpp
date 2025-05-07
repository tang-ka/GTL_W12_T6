
#include "Asset/SkeletalMeshAsset.h"
#include "SkeletalMesh.h"


USkeletalMesh::~USkeletalMesh()
{
    OutputDebugStringA("USkeletalMesh destroyed");
}

void USkeletalMesh::SetRenderData(std::unique_ptr<FSkeletalMeshRenderData> InRenderData)
{
    RenderData = std::move(InRenderData);
}

const FSkeletalMeshRenderData* USkeletalMesh::GetRenderData() const
{
    return RenderData.get(); 
}
