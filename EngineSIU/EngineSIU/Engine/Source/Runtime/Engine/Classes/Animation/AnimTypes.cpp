#include "AnimTypes.h"
#include "Animation/AnimNotifyState.h"

void FAnimNotifyEvent::SetAnimNotify(class UAnimNotify* InNotify)
{
    if (Notify)
    {
        delete Notify;
    }
    Notify = InNotify;
}

void FAnimNotifyEvent::SetAnimNotifyState(class UAnimNotifyState* InNotifyState)
{
    if (NotifyState)
    {
        delete NotifyState;
    }
    NotifyState = InNotifyState;
}
