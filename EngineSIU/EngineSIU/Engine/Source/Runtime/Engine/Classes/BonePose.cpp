#include "BonePose.h"

void FCompactPose::ResetToAdditiveIdentity()
{
    for (FTransform& Bone : this->Bones)
    {
        Bone.SetIdentityZeroScale();
    }
}

void FCompactPose::NormalizeRotations()
{
    for (FTransform& Bone : this->Bones)
    {
        Bone.NormalizeRotation();
    }
}
