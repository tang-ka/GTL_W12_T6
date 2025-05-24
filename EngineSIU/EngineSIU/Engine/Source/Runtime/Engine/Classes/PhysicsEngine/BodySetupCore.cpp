#include "BodySetupCore.h"

UBodySetupCore::UBodySetupCore()
{
    CollisionTraceFlag = CTF_UseDefault;
}

ECollisionTraceFlag UBodySetupCore::GetCollisionTraceFlag() const
{
    return CollisionTraceFlag;
}
