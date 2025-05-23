﻿#pragma once
#include "Container/String.h"
#include "Misc/Optional.h"


struct FPropertyMetadata
{
    // 카테고리
    // TOptional<FString> Category = {}; // TODO: 카테고리 만들기

    // UI에 실제로 보여지는 이름
    TOptional<FString> DisplayName = {};

    // 툴팁
    TOptional<FString> ToolTip = {};

    /* Ex
     * UPROPERTY(EditAnywhere, Category = "Move")
     * bool bCanMove = true; 
     * UPROPERTY(EditAnywhere, Category = "Move", meta = (EditCondition = "bCanMove"))
     * int MoveSpeed = 100;
     * // 숨김 옵션
     * UPROPERTY(EditAnywhere, Category = "Move", meta = (EditCondition = "bCanMove", EditConditionHides))
     * int MoveSpeed = 100;
     * // Enum
     * UENUM(BlueprintType)
     * enum class ETestType : uint8 { A_Type, B_Type };
     * UPROPERTY(EditAnywhere, Category = "Move")
     * ETestType TestType = ETestType::A_Type;
     * UPROPERTY(EditAnywhere, Category = "Move", meta = (EditCondition = "TestType == ETestType::A_Type"))
     * float A_Speed = 10.0f;
     * UPROPERTY(EditAnywhere, Category = "Move", meta = (EditCondition = "TestType == ETestType::B_Type"))
     * float B_Speed = 1.0f;
     */
    // 특정 Property의 값에 따라서 프로퍼티의 에디터 노출 및 편집 가능 여부를 제어
    // TOptional<?> EditCondition = {};

    // DisplayAfter / DisplayPriority: 에디터에서 표시 순서 제어

    // ArrayClamp 정수형 프로퍼티, 최대 Idx 제한

    // bool Type에 대해서 UI상에 ImGui::SameLine을 적용할지 여부
    bool InlineEditConditionToggle = false;

    // +/- 의 증감 단위
    // TOptional<float> Delta;

    // ImGui::DragScalar의 속도
    float DragDeltaSpeed = 0.1f;

    // 실제 슬라이더의 값을 설정할 수 있는 최소치, (None이면 실제 타입의 Min)
    TOptional<float> ClampMin = {};

    // 실제 슬라이더의 값을 설정할 수 있는 최대치, (None이면 실제 타입의 Max)
    TOptional<float> ClampMax = {};

    // // UI상에서 마우스로 값을 조절할 수 있는 최소치, (None이면 실제 타입의 Min)
    // TOptional<float> UIMin = {};
    //
    // // UI상에서 마우스로 값을 조절할 수 있는 최대치, (None이면 실제 타입의 Max)
    // TOptional<float> UIMax = {};

    // FColor, FLinearColor 등에서 알파 채널 숨김
    bool HideAlphaChannel = false;
};
