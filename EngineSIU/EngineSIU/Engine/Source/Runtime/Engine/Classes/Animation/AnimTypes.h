#pragma once
#include "Container/Array.h"
#include "UObject/NameTypes.h"
#include "Math/Quat.h"
#include "Math/Vector.h"

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
    float GetEndTime() const { return Time + Duration;}
    UAnimNotify* GetNotify() const { return Notify; }
    UAnimNotifyState* GetNotifyState() const { return NotifyState; }
    void SetAnimNotify(class UAnimNotify* InNotify);
    void SetAnimNotifyState(class UAnimNotifyState* InNotifyState);
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

    void Serialize(FArchive& Ar)
    {
        Ar << PosKeys << RotKeys << ScaleKeys;
    }
};

// From Unreal Engine IAnimationDataMode.h
struct FBoneAnimationTrack
{
    FRawAnimSequenceTrack InternalTrackData;

    int32 BoneTreeIndex = INDEX_NONE;

    FName Name;

    friend FArchive& operator<<(FArchive& Ar, FBoneAnimationTrack& Data)
    {
        Data.InternalTrackData.Serialize(Ar);

        return Ar << Data.BoneTreeIndex << Data.Name;
    }
};

enum class EAnimInterpolationType : uint8
{
    /** Linear interpolation when looking up values between keys. */
    Linear,

    /** Step interpolation when looking up values between keys. */
    Step,
};
