#pragma once
#include "Define.h"

inline FVertexTexture QuadVertices[] =
{
    {-1.f, 1.f, 0.f, 0.f, 0.f},
    {1.f, 1.f, 0.f, 1.f, 0.f },
    {-1.f, -1.f, 0.f, 0.f, 1.f},
    {1.f, -1.f, 0.f, 1.f, 1.f}
};

inline uint32 QuadIndices[] =
{
    0,1,2,
    2,1,3
};
