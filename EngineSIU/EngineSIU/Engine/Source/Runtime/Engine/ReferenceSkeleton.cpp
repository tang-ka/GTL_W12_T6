
#include "ReferenceSkeleton.h"

#include "Math/Matrix.h"

int32 FReferenceSkeleton::FindBoneIndex(const FName& BoneName) const
{
    if (RawNameToIndexMap.Contains(BoneName))
    {
        return RawNameToIndexMap[BoneName];
    }
    return INDEX_NONE;
}

void FReferenceSkeleton::InitializeInverseBindPoseMatrices()
{
    const int32 BoneCount = RawRefBoneInfo.Num();
    InverseBindPoseMatrices.SetNum(BoneCount);
        
    // 본 변환 행렬 배열 (계층 구조 고려)
    TArray<FMatrix> BindPoseMatrices;
    BindPoseMatrices.SetNum(BoneCount);
        
    // 계층 구조를 고려한 바인드 포즈 행렬 계산
    for (int32 BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
    {
        // 현재 본의 로컬 변환
        FMatrix LocalMatrix = RawRefBonePose[BoneIndex].ToMatrixWithScale();
            
        // 부모 본의 영향을 적용
        int32 ParentIndex = RawRefBoneInfo[BoneIndex].ParentIndex;
        if (ParentIndex != INDEX_NONE)
        {
            // 부모 행렬과 현재 로컬 행렬을 곱하여 월드 변환 계산
            BindPoseMatrices[BoneIndex] = LocalMatrix * BindPoseMatrices[ParentIndex];
        }
        else
        {
            // 루트 본은 로컬 변환이 곧 월드 변환
            BindPoseMatrices[BoneIndex] = LocalMatrix;
        }
            
        // 역행렬 계산 및 저장
        InverseBindPoseMatrices[BoneIndex] = FMatrix::Inverse(BindPoseMatrices[BoneIndex]);
    }
}
