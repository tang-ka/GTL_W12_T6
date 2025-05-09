
#include "AnimSequenceBase.h"

#include "UObject/ObjectFactory.h"
#include "Animation/AnimData/AnimDataModel.h"

UAnimSequenceBase::UAnimSequenceBase()
{
    CreateModel();
}

UAnimSequenceBase::~UAnimSequenceBase()
{
    if (DataModel)
    {
        delete DataModel;
        DataModel = nullptr;
    }
}

float UAnimSequenceBase::GetPlayLength() const
{
    return SequenceLength;
}

UAnimDataModel* UAnimSequenceBase::GetDataModel() const
{
    return DataModel;
}

void UAnimSequenceBase::CreateModel()
{
    DataModel = FObjectFactory::ConstructObject<UAnimDataModel>(this);
}
