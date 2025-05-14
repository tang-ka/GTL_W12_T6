#include "ImGuiWidget.h"
#include <array>
#include "Math/Rotator.h"
#include "Math/Vector.h"

bool FImGuiWidget::DrawVec3Control(const std::string& Label, FVector& Values, float ResetValue, float ColumnWidth)
{
    return DisplayNControl<3>(Label, ResetValue, ColumnWidth, "%.2f",
    {{
        {
            .Label = "X",
            .ButtonColor = {1.0f, 0.0f, 0.0f, 1.0f},
            .ButtonHoveredColor = {0.9f, 0.2f, 0.2f, 1.0f},
            .ButtonActiveColor = {0.8f, 0.1f, 0.15f, 1.0f},
            .ValuePtr = &Values.X
        },
        {
            .Label = "Y",
            .ButtonColor = {0.0f, 0.5f, 0.0f, 1.0f},
            .ButtonHoveredColor = {0.3f, 0.8f, 0.3f, 1.0f},
            .ButtonActiveColor = {0.2f, 0.7f, 0.2f, 1.0f}, 
            .ValuePtr = &Values.Y
        },
        {
            .Label = "Z",
            .ButtonColor = {0.0f, 0.0f, 1.0f, 1.0f},
            .ButtonHoveredColor = {0.2f, 0.35f, 0.9f, 1.0f},
            .ButtonActiveColor = {0.1f, 0.25f, 0.8f, 1.0f}, 
            .ValuePtr = &Values.Z
        }
    }});
}

bool FImGuiWidget::DrawRot3Control(const std::string& Label, FRotator& Values, float ResetValue, float ColumnWidth)
{
    return DisplayNControl<3>(Label, ResetValue, ColumnWidth, "%.2f°",
    {{
        {
            .Label = "Roll",
            .ButtonColor = {1.0f, 0.0f, 0.0f, 1.0f},
            .ButtonHoveredColor = {0.9f, 0.2f, 0.2f, 1.0f}, 
            .ButtonActiveColor = {0.8f, 0.1f, 0.15f, 1.0f},
            .ValuePtr = &Values.Roll
        },
        {
            .Label = "Pitch", 
            .ButtonColor = {0.0f, 0.5f, 0.0f, 1.0f},
            .ButtonHoveredColor = {0.3f, 0.8f, 0.3f, 1.0f},
            .ButtonActiveColor = {0.2f, 0.7f, 0.2f, 1.0f},
            .ValuePtr = &Values.Pitch
        },
        {
            .Label = "Yaw",
            .ButtonColor = {0.0f, 0.0f, 1.0f, 1.0f}, 
            .ButtonHoveredColor = {0.2f, 0.35f, 0.9f, 1.0f},
            .ButtonActiveColor = {0.1f, 0.25f, 0.8f, 1.0f},
            .ValuePtr = &Values.Yaw
        }
    }});
}
