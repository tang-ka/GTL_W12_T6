
#include "AnimSequenceBase.h"

#include "UObject/ObjectFactory.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Developer/AnimDataController/AnimDataController.h"
#include "Animation/AnimTypes.h"

UAnimSequenceBase::UAnimSequenceBase()
    : RateScale(1.f)
    , bLoop(true)
    , DataModel(nullptr)
    , Controller(nullptr)
{
    CreateModel();
    GetController();
}

UAnimSequenceBase::~UAnimSequenceBase()
{
    if (DataModel)
    {
        delete DataModel;
        DataModel = nullptr;
    }

    if (Controller)
    {
        delete Controller;
        Controller = nullptr;
    }
}

float UAnimSequenceBase::GetPlayLength() const
{
    if (DataModel)
    {
        return static_cast<float>(DataModel->GetPlayLength());   
    }
    return 0.f;
}

UAnimDataModel* UAnimSequenceBase::GetDataModel() const
{
    return DataModel;
}

UAnimDataController& UAnimSequenceBase::GetController()
{
    if (Controller == nullptr)
    {
        Controller = DataModel->GetController();
        Controller->SetModel(DataModel);
    }

    return *Controller;
}

void UAnimSequenceBase::CreateModel()
{
    DataModel = FObjectFactory::ConstructObject<UAnimDataModel>(this);
}

bool UAnimSequenceBase::AddNotifyTrack(const FName& TrackName, int32& OutNewTrackIndex)
{
    OutNewTrackIndex = INDEX_NONE;
    if (TrackName.ToString().IsEmpty())
    {
        return false;
    }
    if (FindNotifyTrackIndex(TrackName) != INDEX_NONE)
    {
        return false;
    }
    FAnimNotifyTrack NewTrack(TrackName);
    OutNewTrackIndex = AnimNotifyTracks.Add(NewTrack);
    return true;
}

bool UAnimSequenceBase::RemoveNotifyTrack(int32 TrackIndexToRemove)
{
    if (!AnimNotifyTracks.IsValidIndex(TrackIndexToRemove))
    {
        return false;
    }

    // Remove all notifies associated with this track from the global Notifies array
    // Also, adjust TrackIndex for notifies on tracks that are shifted
    for (int32 NotifyIdx = Notifies.Num() - 1; NotifyIdx >= 0; --NotifyIdx)
    {
        if (Notifies[NotifyIdx].TrackIndex == TrackIndexToRemove)
        {
            Notifies.RemoveAt(NotifyIdx); 
        }
        else if (Notifies[NotifyIdx].TrackIndex > TrackIndexToRemove)
        {
            Notifies[NotifyIdx].TrackIndex--;
        }
    }
    AnimNotifyTracks.RemoveAt(TrackIndexToRemove);
    return true;
}

bool UAnimSequenceBase::RenameNotifyTrack(int32 TrackIndex, const FName& NewTrackName)
{
    if (!AnimNotifyTracks.IsValidIndex(TrackIndex))
    {
        return false;
    }
    if (NewTrackName.ToString().IsEmpty())
    {
        return false;
    }
    int32 ExistingTrackIndex = FindNotifyTrackIndex(NewTrackName);
    if (ExistingTrackIndex != INDEX_NONE && ExistingTrackIndex != TrackIndex)
    {
        return false;
    }
    AnimNotifyTracks[TrackIndex].TrackName = NewTrackName;
    return true;
}

bool UAnimSequenceBase::AddNotifyEvent(int32 TargetTrackIndex, float Time, float Duration, const FName& NotifyName, int32& OutNewNotifyIndex)
{
    OutNewNotifyIndex = INDEX_NONE;
    if (!AnimNotifyTracks.IsValidIndex(TargetTrackIndex))
    {
        return false;
    }
    if (NotifyName.ToString().IsEmpty())
    {
        return false;
    }

    FAnimNotifyEvent NewEvent;
    NewEvent.Time = Time;
    NewEvent.Duration = Duration;
    NewEvent.TrackIndex = TargetTrackIndex;
    NewEvent.NotifyName = NotifyName;
    OutNewNotifyIndex = Notifies.Add(NewEvent);

    // Add the global notify index to the track's list of notifies
    AnimNotifyTracks[TargetTrackIndex].NotifyIndices.Add(OutNewNotifyIndex);
    return true;
}

bool UAnimSequenceBase::RemoveNotifyEvent(int32 NotifyIndexToRemove)
{
    if (!Notifies.IsValidIndex(NotifyIndexToRemove))
    {
        return false;
    }

    const FAnimNotifyEvent& EventToRemove = Notifies[NotifyIndexToRemove];
    if (AnimNotifyTracks.IsValidIndex(EventToRemove.TrackIndex))
    {
        // Remove the global notify index from its track's list
        AnimNotifyTracks[EventToRemove.TrackIndex].NotifyIndices.RemoveSingle(NotifyIndexToRemove); 
    }

    Notifies.RemoveAt(NotifyIndexToRemove);
    
    // Adjust NotifyIndices in all tracks for global indices that shifted
    for (FAnimNotifyTrack& Track : AnimNotifyTracks)
    {
        for (int32 i = Track.NotifyIndices.Num() - 1; i >= 0; --i)
        {
            if (Track.NotifyIndices[i] > NotifyIndexToRemove)
            {
                Track.NotifyIndices[i]--;
            }
            // If an index became invalid somehow (e.g. points to the removed one, though RemoveAt handles shifting), clean up.
            // This check might be overly cautious if RemoveAt and the loop above are correct.
            else if (Track.NotifyIndices[i] == NotifyIndexToRemove) 
            {
                 Track.NotifyIndices.RemoveAt(i); // Should have been caught by RemoveSingle if it was the one
            }
        }
    }
    return true;
}

bool UAnimSequenceBase::UpdateNotifyEvent(int32 NotifyIndexToUpdate, float NewTime, float NewDuration, int32 NewTrackIndex, const FName& NewNotifyName)
{
    if (!Notifies.IsValidIndex(NotifyIndexToUpdate))
    {
        return false;
    }
    if (!AnimNotifyTracks.IsValidIndex(NewTrackIndex))
    {
        return false;
    }

    FAnimNotifyEvent& EventToUpdate = Notifies[NotifyIndexToUpdate];
    int32 OldTrackIndex = EventToUpdate.TrackIndex;

    EventToUpdate.Time = NewTime;
    EventToUpdate.Duration = NewDuration;
    
    if (OldTrackIndex != NewTrackIndex)
    {
        if (AnimNotifyTracks.IsValidIndex(OldTrackIndex))
        {
            AnimNotifyTracks[OldTrackIndex].NotifyIndices.RemoveSingle(NotifyIndexToUpdate);
        }
        AnimNotifyTracks[NewTrackIndex].NotifyIndices.Add(NotifyIndexToUpdate); 
        EventToUpdate.TrackIndex = NewTrackIndex;
    }

    if (!NewNotifyName.ToString().IsEmpty())
    {
        EventToUpdate.NotifyName = NewNotifyName;
    }
    return true;
}

FAnimNotifyEvent* UAnimSequenceBase::GetNotifyEvent(int32 NotifyIndex)
{
    if (Notifies.IsValidIndex(NotifyIndex))
    {
        return &Notifies[NotifyIndex];
    }
    return nullptr;
}

const FAnimNotifyEvent* UAnimSequenceBase::GetNotifyEvent(int32 NotifyIndex) const
{
    if (Notifies.IsValidIndex(NotifyIndex))
    {
        return &Notifies[NotifyIndex];
    }
    return nullptr;
}

int32 UAnimSequenceBase::FindNotifyTrackIndex(const FName& TrackName) const
{
    for (int32 Index = 0; Index < AnimNotifyTracks.Num(); ++Index)
    {
        if (AnimNotifyTracks[Index].TrackName == TrackName)
        {
            return Index;
        }
    }
    return INDEX_NONE;
}
