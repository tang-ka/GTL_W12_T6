#pragma once
#include "IWindowToggleable.h"
#include "UnrealEd/EditorViewportClient.h"
class ViewportTypePanel : public IWindowToggleable
{

public:
    static ViewportTypePanel& GetInstance();

    void Draw(const std::shared_ptr<FEditorViewportClient>& ActiveViewport) const;
    void OnResize(HWND hWnd);

    virtual void Toggle() override {
        if (bWasOpen) {
            bWasOpen = false;
        }
    }

private:
    bool bWasOpen = true;
    UINT width;
    UINT height;
};

