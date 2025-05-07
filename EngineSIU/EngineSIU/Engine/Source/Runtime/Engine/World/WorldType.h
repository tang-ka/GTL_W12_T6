#pragma once

enum class EWorldType
{
    None,
    Game,
    Editor,
    PIE,
    EditorPreview,
    GamePreview,
    GameRPC,
    SkeletalViewer,
    Inactive
};

// TODO : EWolrldType과 EWorldTypeBitFlag 혼용 중. EWorldTypeBitFlag으로 통일하기 
enum class EWorldTypeBitFlag
{
    None = 0,
    Game = 1<<0,
    Editor = 1<<1,
    PIE = 1<<2,
    EditorPreview = 1<<3,
    GamePreview = 1<<4,
    GameRPC = 1<<5,
    SkeletalViewer = 1<<6,
    Inactive = 1<<7
};

inline EWorldTypeBitFlag operator|(EWorldTypeBitFlag a, EWorldTypeBitFlag b) {
    return static_cast<EWorldTypeBitFlag>(
        uint32(a) | uint32(b)
        );
}
inline EWorldTypeBitFlag operator&(EWorldTypeBitFlag a, EWorldTypeBitFlag b) {
    return static_cast<EWorldTypeBitFlag>(
        uint32(a) & uint32(b)
        );
}
inline bool HasFlag(EWorldTypeBitFlag mask, EWorldTypeBitFlag flag) {
    return (uint32(mask) & uint32(flag)) != 0;
}
