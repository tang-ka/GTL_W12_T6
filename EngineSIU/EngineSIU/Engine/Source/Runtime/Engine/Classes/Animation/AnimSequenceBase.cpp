
#include "AnimSequenceBase.h"

#include "UObject/ObjectFactory.h"
#include "Animation/AnimData/AnimDataModel.h"
#include "Developer/AnimDataController/AnimDataController.h"

UAnimSequenceBase::UAnimSequenceBase()
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
    return SequenceLength;
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
