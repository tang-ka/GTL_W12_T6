
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
