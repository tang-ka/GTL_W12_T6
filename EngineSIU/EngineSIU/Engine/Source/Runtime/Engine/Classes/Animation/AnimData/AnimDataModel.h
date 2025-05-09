
#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FBoneAnimationTrack;

class UAnimDataModel : public UObject
{
    DECLARE_CLASS(UAnimDataModel, UObject)
    
public:
    UAnimDataModel();
    virtual ~UAnimDataModel() override = default;

    virtual const TArray<FBoneAnimationTrack>& GetBoneAnimationTracks() const;
    virtual const FBoneAnimationTrack& GetBoneTrackByIndex(int32 TrackIndex) const;
    virtual const FBoneAnimationTrack& GetBoneTrackByName(const FName& TrackName) const;
    virtual const FBoneAnimationTrack* FindBoneTrackByName(FName Name) const;
    virtual const FBoneAnimationTrack* FindBoneTrackByIndex(int32 BoneIndex) const;
    virtual int32 GetBoneTrackIndex(const FBoneAnimationTrack& Track) const;
    virtual int32 GetBoneTrackIndexByName(FName TrackName) const;
    virtual bool IsValidBoneTrackIndex(int32 TrackIndex) const;

    virtual int32 GetFrameRate() const;
    virtual int32 GetNumberOfFrames() const;
    virtual int32 GetNumberOfKeys() const;
    
private:
    // All individual bone animation tracks
    TArray<FBoneAnimationTrack> BoneAnimationTracks;

    // Rate at which the animated data is sampled
    int32 FrameRate;

    // Total number of sampled animated frames
    int32 NumberOfFrames;

    // Total number of sampled animated keys
    int32 NumberOfKeys;
};
