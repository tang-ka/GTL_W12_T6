
#include "AnimSequenceBase.h"

float UAnimSequenceBase::GetPlayLength() const
{
    return SequenceLength;
}

UAnimDataModel* UAnimSequenceBase::GetDataModel() const
{
    return DataModel;
}
