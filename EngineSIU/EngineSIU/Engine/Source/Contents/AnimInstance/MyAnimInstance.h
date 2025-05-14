#pragma once
#include "Animation/Animinstance.h"
#include "UObject/ObjectMacros.h"

class UAnimSequence;
class UAnimationAsset;

class UMyAnimInstance : public UAnimInstance
{
    DECLARE_CLASS(UMyAnimInstance, UAnimInstance)

public:
    UMyAnimInstance();
    
    UAnimSequence* AnimA;
    
    UAnimSequence* AnimB;
    
    float BlendAlpha = 0.f;
    
    virtual void SetAnimationAsset(UAnimationAsset* NewAsset, bool bIsLooping=true, float InPlayRate=1.f);
    
    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds, FPoseContext& OutPose) override;
    
    UAnimationAsset* GetAnimationAsset() const
    {
        return CurrentAsset;
    }
    
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
    UAnimationAsset* CurrentAsset;
    //어차피 상속받아서 직접 만든 클래스니까 상태별로 Asset 저장해두고 써도 되지 않을까 싶음.

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
