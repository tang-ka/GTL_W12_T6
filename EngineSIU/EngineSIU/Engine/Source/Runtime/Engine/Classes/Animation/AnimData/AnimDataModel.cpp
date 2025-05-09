#include "AnimDataModel.h"

#include "Animation/AnimTypes.h"
#include "Container/Array.h"

UAnimDataModel::UAnimDataModel()
    : BoneAnimationTracks()
    , FrameRate(0)
    , NumberOfFrames(0)
    , NumberOfKeys(0)
{
}

const TArray<FBoneAnimationTrack>& UAnimDataModel::GetBoneAnimationTracks() const
{
    return BoneAnimationTracks;
}

const FBoneAnimationTrack& UAnimDataModel::GetBoneTrackByIndex(int32 TrackIndex) const
{
    return BoneAnimationTracks[TrackIndex];
}

const FBoneAnimationTrack& UAnimDataModel::GetBoneTrackByName(const FName& TrackName) const
{
    const FBoneAnimationTrack* TrackPtr = BoneAnimationTracks.FindByPredicate([TrackName](const FBoneAnimationTrack& Track)
    {
        return Track.Name == TrackName;
    });

	return *TrackPtr;
}

const FBoneAnimationTrack* UAnimDataModel::FindBoneTrackByName(FName Name) const
{
    return BoneAnimationTracks.FindByPredicate([Name](const FBoneAnimationTrack& Track)
    {
        return Track.Name == Name;
    });
}

const FBoneAnimationTrack* UAnimDataModel::FindBoneTrackByIndex(int32 BoneIndex) const
{
    const FBoneAnimationTrack* TrackPtr = BoneAnimationTracks.FindByPredicate([BoneIndex](const FBoneAnimationTrack& Track)
	{
		return Track.BoneTreeIndex == BoneIndex;
	});

	return TrackPtr;
}

int32 UAnimDataModel::GetBoneTrackIndex(const FBoneAnimationTrack& Track) const
{
    return BoneAnimationTracks.IndexOfByPredicate([&Track](const FBoneAnimationTrack& SearchTrack)
    {
        return SearchTrack.Name == Track.Name;
    });
}

int32 UAnimDataModel::GetBoneTrackIndexByName(FName TrackName) const
{
    if (const FBoneAnimationTrack* TrackPtr = FindBoneTrackByName(TrackName))
    {
        return GetBoneTrackIndex(*TrackPtr);
    }

    return INDEX_NONE;
}

bool UAnimDataModel::IsValidBoneTrackIndex(int32 TrackIndex) const
{
    return BoneAnimationTracks.IsValidIndex(TrackIndex);
}

int32 UAnimDataModel::GetFrameRate() const
{
    return FrameRate;
}

int32 UAnimDataModel::GetNumberOfFrames() const
{
    return NumberOfFrames;
}

int32 UAnimDataModel::GetNumberOfKeys() const
{
    return NumberOfKeys;
}
