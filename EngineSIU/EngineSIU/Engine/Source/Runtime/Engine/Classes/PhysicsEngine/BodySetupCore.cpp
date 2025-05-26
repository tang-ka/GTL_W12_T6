#include "BodySetupCore.h"

UBodySetupCore::UBodySetupCore()
{
    CollisionTraceFlag = CTF_UseDefault;
}

void UBodySetupCore::GetProperties(TMap<FString, FString>& OutProperties) const
{
    //OutProperties.Add(TEXT("FinalIndexU"), FString::Printf(TEXT("%f"), FinalIndexU));
}

void UBodySetupCore::SetProperties(const TMap<FString, FString>& InProperties)
{
    /*const FString* TempStr = nullptr;
    TempStr = InProperties.Find(TEXT("FinalIndexU"));
    if (TempStr)
    {
        FinalIndexU = FString::ToFloat(*TempStr);
    }*/
}

ECollisionTraceFlag UBodySetupCore::GetCollisionTraceFlag() const
{
    return CollisionTraceFlag;
}
