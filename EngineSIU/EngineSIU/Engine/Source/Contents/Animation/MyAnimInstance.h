#pragma once
#include "Animation/AnimInstance.h"

class UAnimStateMachine;
class UAnimationAsset;

class UMyAnimInstance : public UAnimInstance
{
    DECLARE_CLASS(UMyAnimInstance, UAnimInstance)
public:
    UMyAnimInstance();
    virtual ~UMyAnimInstance() override = default;
    
    virtual void NativeInitializeAnimation() override;
    
    virtual void NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose) override;

    UAnimationAsset* GetAnimationAsset() const;

    void SetAnimationAsset(UAnimationAsset* InAnimationAsset);
    
    void SetPlaying(bool bIsPlaying)
    {
        bPlaying = bIsPlaying;
    }

    bool IsPlaying() const
    {
        return bPlaying;
    }

    void SetReverse(bool bIsReverse)
    {
        bReverse = bIsReverse;
    }
    
    bool IsReverse() const
    {
        return bReverse;
    }

    void SetLooping(bool bIsLooping)
    {
        bLooping = bIsLooping;
    }

    bool IsLooping() const
    {
        return bLooping;
    }

    void SetElapsedTime(float InElapsedTime)
    {
        ElapsedTime = InElapsedTime;
    }

    float GetElapsedTime() const
    {
        return ElapsedTime;
    }

    float GetPlayRate() const
    {
        return PlayRate;
    }

    void SetPlayRate(float InPlayRate)
    {
        PlayRate = InPlayRate;
    }

    int32 GetLoopStartFrame() const
    {
        return LoopStartFrame;
    }

    void SetLoopStartFrame(int32 InLoopStartFrame)
    {
        LoopStartFrame = InLoopStartFrame;
    }

    int32 GetLoopEndFrame() const
    {
        return LoopEndFrame;
    }

    void SetLoopEndFrame(int32 InLoopEndFrame)
    {
        LoopEndFrame = InLoopEndFrame;
    }

    int GetCurrentKey() const
    {
        return CurrentKey;
    }

    void SetCurrentKey(int InCurrentKey)
    {
        CurrentKey = InCurrentKey;
    }

private:
    UAnimStateMachine* StateMachine;
    class UAnimationAsset;
    
    UAnimationAsset* CurrentAsset;
    
    float ElapsedTime;

    float PreviousTime;
    
    float PlayRate;
    
    bool bLooping;
    
    bool bPlaying;
    
    bool bReverse;

    int32 LoopStartFrame;

    int32 LoopEndFrame;

    int CurrentKey;
};

