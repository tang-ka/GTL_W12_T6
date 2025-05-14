#pragma once
#include "Core/HAL/PlatformType.h"

class UImGuiManager
{
public:
    void Initialize(HWND hWnd, ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
    void BeginFrame() const;
    void EndFrame() const;
    void PreferenceStyle() const;
    void Shutdown();
};

