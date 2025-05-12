#pragma once
#include "Container/Array.h"
#include "UObject/NameTypes.h"

struct FQuat;
struct FVector;

#define MAX_ANIMATION_TRACKS 65535

struct FAnimNotifyEvent
{
    float Time = 0.0f;                
    float Duration = 0.0f;            
    int32 TrackIndex = 0;             
    FName NotifyName;                 

    class UAnimNotify* Notify = nullptr;             
    class UAnimNotifyState* NotifyState = nullptr;

    bool bTriggered = false; 
    bool bStateActive = false;
    
    bool IsState() const { return Duration > 0.f; }
    float GetEndTime() const { return Time + Duration; }
};

// FAnimNotifyTrack:노티파이 트랙 단위
struct FAnimNotifyTrack
{
    FName TrackName;                 
    TArray<int32> NotifyIndices;    

    FAnimNotifyTrack() = default;

    FAnimNotifyTrack(FName InName)
        : TrackName(InName)
    {}
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
