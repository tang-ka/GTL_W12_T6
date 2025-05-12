#pragma once
#include "AnimationAsset.h"

class UAnimDataController;
class UAnimDataModel;
struct FAnimNotifyEvent;
struct FAnimNotifyTrack;

class UAnimSequenceBase : public UAnimationAsset
{
    DECLARE_CLASS(UAnimSequenceBase, UAnimationAsset)

public:
    UAnimSequenceBase();
    virtual ~UAnimSequenceBase() override;

    TArray<FAnimNotifyEvent> Notifies;

    TArray<FAnimNotifyTrack> AnimNotifyTracks;

    float RateScale;

    bool bLoop;

protected:
    UAnimDataModel* DataModel;

    UAnimDataController* Controller;

public:
    virtual float GetPlayLength() const override;

    UAnimDataModel* GetDataModel() const;

    UAnimDataController& GetController();

    TArray<FAnimNotifyTrack>& GetAnimNotifyTracks() 
    {
        return AnimNotifyTracks;
    }

private:
    void CreateModel();
};
