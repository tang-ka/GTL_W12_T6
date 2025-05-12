#pragma once
#include "AnimInstance.h"
#include "UObject/ObjectMacros.h"


class UAnimationAsset;

class UAnimSingleNodeInstance : public UAnimInstance
{
    DECLARE_CLASS(UAnimSingleNodeInstance, UAnimInstance)

public:
    UAnimSingleNodeInstance();

    virtual void SetAnimationAsset(UAnimationAsset* NewAsset, bool bIsLooping=true, float InPlayRate=1.f);

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

    void SetCurrentTime(float InCurrentTime)
    {
        CurrentTime = InCurrentTime;
    }

    float GetCurrentTime() const
    {
        return CurrentTime;
    }

    float GetPlayRate() const
    {
        return PlayRate;
    }

    void SetPlayRate(float InPlayRate)
    {
        PlayRate = InPlayRate;
    }

private:
    UAnimationAsset* CurrentAsset;
    
    float CurrentTime;
    
    float PlayRate;
    
    bool bLooping;
    
    bool bPlaying;
    
    bool bReverse;
};
