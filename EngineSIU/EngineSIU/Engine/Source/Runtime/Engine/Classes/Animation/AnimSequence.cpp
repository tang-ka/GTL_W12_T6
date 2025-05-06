
#include "AnimSequence.h"

#include "Math/Transform.h"

UAnimSequence::UAnimSequence()
{
    for (int32 i = 0; i < NumFrames; ++i)
    {
        TMap<int32, FTransform> Track;
        for (int32 j = 0; j < 4; ++j)
        {
            FTransform TF;
            TF.Rotation = FRotator(FMath::Sin((static_cast<float>(i) / FrameRate) * PI * 1.f) * 15.f, 0.f, 0.f).Quaternion();
            Track.Add(j, TF);
        }
        Anim.Add(Track);
    }
}
