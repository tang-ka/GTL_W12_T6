#pragma once

#include "UObject/ObjectMacros.h"

enum ECollisionTraceFlag : int
{
    /** Use project physics settings (DefaultShapeComplexity) */
    CTF_UseDefault,     /** Create both simple and complex shapes. Simple shapes are used for regular scene queries and collision tests. Complex shape (per poly) is used for complex scene queries.*/
    CTF_UseSimpleAndComplex,
    /** Create only simple shapes. Use simple shapes for all scene queries and collision tests.*/
    CTF_UseSimpleAsComplex,
    /** Create only complex shapes (per poly). Use complex shapes for all scene queries and collision tests. Can be used in simulation for static shapes only (i.e can be collided against but not moved through forces or velocity.) */
    CTF_UseComplexAsSimple,
    CTF_MAX,
};

enum EPhysicsType : int
{
    /** Follow owner. */
    PhysType_Default,
    /** Do not follow owner, but make kinematic. */
    PhysType_Kinematic,
    /** Do not follow owner, but simulate. */
    PhysType_Simulated
};

namespace EBodyCollisionResponse
{
    enum Type : int
    {
        BodyCollision_Enabled,
        BodyCollision_Disabled 
    };
}

/** Helpers to convert enum to string */
const TCHAR* LexToString(ECollisionTraceFlag Enum);
const TCHAR* LexToString(EPhysicsType Enum);
const TCHAR* LexToString(EBodyCollisionResponse::Type Enum);
