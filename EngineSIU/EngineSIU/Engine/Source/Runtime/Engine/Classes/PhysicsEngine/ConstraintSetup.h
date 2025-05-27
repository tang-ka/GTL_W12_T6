#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UConstraintSetup : public UObject
{
    DECLARE_CLASS(UConstraintSetup, UObject);

public: 
    UConstraintSetup();
    ~UConstraintSetup() = default;

    virtual void DisplayProperty() override;

    UPROPERTY_WITH_FLAGS(VisibleAnywhere, FName, BoneNameA)
    UPROPERTY_WITH_FLAGS(VisibleAnywhere, FName, BoneNameB)

    UPROPERTY_WITH_FLAGS(EditAnywhere, FTransform, TransformInA)
    UPROPERTY_WITH_FLAGS(EditAnywhere, FTransform, TransformInB)

    UPROPERTY_WITH_FLAGS(EditAnywhere, float, SwingLimitAngle)
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, TwistLimitAngle)

    // --- 선형(위치) 제약 ---
    // 각 축별 모션 타입: Free, Limited, Locked
    //UPROPERTY(EditAnywhere) TEnumAsByte<ELinearConstraintMotion> LinearXMotion;
    //UPROPERTY(EditAnywhere) TEnumAsByte<ELinearConstraintMotion> LinearYMotion;
    //UPROPERTY(EditAnywhere) TEnumAsByte<ELinearConstraintMotion> LinearZMotion;

    //// 제한 모드가 Limited일 때 적용할 최대 이동 거리
    //UPROPERTY(EditAnywhere, meta = (EditCondition = "LinearXMotion==LCM_Limited || LinearYMotion==LCM_Limited || LinearZMotion==LCM_Limited"))
    UPROPERTY_WITH_FLAGS(EditAnywhere, float, LinearLimitSize)
};

