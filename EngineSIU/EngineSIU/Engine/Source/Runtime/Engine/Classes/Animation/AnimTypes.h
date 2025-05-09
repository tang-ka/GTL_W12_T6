#pragma once
#include "Container/Array.h"
#include "UObject/NameTypes.h"

struct FQuat;
struct FVector;

#define MAX_ANIMATION_TRACKS 65535

struct FAnimNotifyEvent
{
    
};

struct FAnimNotifyTrack
{
    
};

struct FRawAnimSequenceTrack
{
    TArray<FVector> PosKeys;

    TArray<FQuat> RotKeys;

    TArray<FVector> ScaleKeys;
};

// From Unreal Engine IAnimationDataMode.h
struct FBoneAnimationTrack
{
    FRawAnimSequenceTrack InternalTrackData;

    int32 BoneTreeIndex = INDEX_NONE;

    FName Name;
};

enum class EAnimInterpolationType : uint8
{
    /** Linear interpolation when looking up values between keys. */
    Linear,

    /** Step interpolation when looking up values between keys. */
    Step,
};
