#include "AnimStateMachine.h"

UAnimStateMachine::UAnimStateMachine()
{
    CurrentState = EAnimState::AS_Idle;
    MoveSpeed = 0;
}

EAnimState UAnimStateMachine::GetState() const
{
    return CurrentState;
}

FString UAnimStateMachine::GetStateName(EAnimState State) const
{
    switch (State)
    {
    case AS_Idle: return TEXT("Idle");
    case AS_Dance: return TEXT("Dance");
    case AS_SlowRun: return TEXT("SlowRun");
    case AS_NarutoRun: return TEXT("NarutoRun");
    case AS_FastRun: return TEXT("FastRun");
    default: return TEXT("Unknown");
    }
}

void UAnimStateMachine::MoveFast()
{
    MoveSpeed++;
    MoveSpeed = FMath::Clamp(MoveSpeed, 0, 2);
}

void UAnimStateMachine::MoveSlow()
{
    MoveSpeed--;
    MoveSpeed = FMath::Clamp(MoveSpeed, 0, 2);
}

void UAnimStateMachine::Dance()
{
    bIsDancing = true;
}

void UAnimStateMachine::StopDance()
{
    bIsDancing = false;
}

void UAnimStateMachine::ProcessState()
{
    if (CurrentState == EAnimState::AS_Idle)
    {
        if (MoveSpeed > 0)
        {
            CurrentState = EAnimState::AS_SlowRun;
        }
    }
    else if (CurrentState == EAnimState::AS_SlowRun)
    {
        if (MoveSpeed == 0)
        {
            CurrentState = EAnimState::AS_Idle;
        }
        else if (MoveSpeed == 2)
        {
            CurrentState = EAnimState::AS_NarutoRun;
        }
    }
    else if (CurrentState == EAnimState::AS_NarutoRun)
    {
        if (MoveSpeed == 1)
        {
            CurrentState = EAnimState::AS_SlowRun;
        }
        else if (MoveSpeed == 3)
        {
            CurrentState = EAnimState::AS_FastRun;
        }
    }
    else if (CurrentState == EAnimState::AS_FastRun)
    {
        if (MoveSpeed == 2)
        {
            CurrentState = EAnimState::AS_NarutoRun;
        }
    }
    else if (CurrentState == EAnimState::AS_Dance)
    {
        if (!bIsDancing)
        {
            CurrentState = EAnimState::AS_Idle;
        }
    }
    if (bIsDancing)
    {
        CurrentState = EAnimState::AS_Dance;
    }
}



