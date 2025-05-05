
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
    
    // 바인드 포즈 행렬 계산 (계층 구조 고려)
    TArray<FMatrix> BindPoseMatrices;
    BindPoseMatrices.SetNum(BoneCount);
    
    for (int32 BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
    {
        // 해당 본의 로컬 변환
        FTransform BoneTransform = RawRefBonePose[BoneIndex];
        
        // 부모 본의 영향 적용
        int32 ParentIndex = RawRefBoneInfo[BoneIndex].ParentIndex;
        if (ParentIndex != INDEX_NONE)
        {
            BoneTransform = BoneTransform * FTransform(BindPoseMatrices[ParentIndex]);
        }
        
        // 월드 변환 행렬 저장
        BindPoseMatrices[BoneIndex] = BoneTransform.ToMatrixWithScale();
        
        // 역행렬 계산 및 저장 (주의: 역행렬 계산 실패 가능성 체크)
        InverseBindPoseMatrices[BoneIndex] = FMatrix::Inverse(BindPoseMatrices[BoneIndex]);
    }
}
