#pragma once
#include "Math/Quat.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class USkeleton : public UObject
{
    DECLARE_CLASS(USkeleton, UObject)

public:
    USkeleton() = default;
    virtual ~USkeleton() override = default;

    struct FBoneNode
    {
        FName Name;
        int32 ParentIndex;
        
        FVector LocalLocation;
        FQuat LocalQuat;
        
        FVector GlobalLocation;
        FQuat GlobalQuat;
    };

    // 뼈 배열
    TArray<FBoneNode> BoneTree;
    
    // 이름으로 뼈 인덱스를 찾는 맵
    TMap<FString, int32> BoneNameToIndexMap;
    
    // 뼈 추가
    int32 AddBone(const FString& InName, int32 ParentIndex, const FTransform& LocalTransform);
    
    // 이름으로 뼈 인덱스 찾기
    int32 FindBoneIndex(const FString& BoneName) const;
    
    // 스켈레톤 데이터 초기화
    void Initialize();
};
