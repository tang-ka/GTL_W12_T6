#pragma once
#include "Core/HAL/PlatformType.h"

namespace EEndPlayReason
{
    enum Type : uint8
    {
        /** 명시적인 삭제가 일어났을 때, Destroy()등 */
        Destroyed,
        /** World가 바뀌었을 때 */
        WorldTransition,
        /** 프로그램을 종료했을 때 */
        Quit,
    };
}

namespace ECollisionEnabled
{
    enum Type : int
    {
        /** Will not create any representation in the physics engine. Cannot be used for spatial queries (raycasts, sweeps, overlaps) or simulation (rigid body, constraints). Best performance possible (especially for moving objects) */
        NoCollision,
        /** Only used for spatial queries (raycasts, sweeps, and overlaps). Cannot be used for simulation (rigid body, constraints). Useful for character movement and things that do not need physical simulation. Performance gains by keeping data out of simulation tree. */
        QueryOnly ,
        /** Only used only for physics simulation (rigid body, constraints). Cannot be used for spatial queries (raycasts, sweeps, overlaps). Useful for jiggly bits on characters that do not need per bone detection. Performance gains by keeping data out of query tree */
        PhysicsOnly,
        /** Can be used for both spatial queries (raycasts, sweeps, overlaps) and simulation (rigid body, constraints). */
        QueryAndPhysics,
        /** Only used for probing the physics simulation (rigid body, constraints). Cannot be used for spatial queries (raycasts,
        sweeps, overlaps). Useful for when you want to detect potential physics interactions and pass contact data to hit callbacks
        or contact modification, but don't want to physically react to these contacts. */
        ProbeOnly,
        /** Can be used for both spatial queries (raycasts, sweeps, overlaps) and probing the physics simulation (rigid body,
        constraints). Will not allow for actual physics interaction, but will generate contact data, trigger hit callbacks, and
        contacts will appear in contact modification. */
        QueryAndProbe
    };
}
