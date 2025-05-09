#pragma once
#include "StaticMesh.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

class UAnimationAsset;
class USkeleton;
class USkeletalMesh;

enum class EAssetType : uint8
{
    StaticMesh,
    SkeletalMesh,
    Skeleton,
    Animation,
    Texture2D,
    Material,
};

struct FAssetInfo
{
    FName AssetName;      // Asset의 이름
    FName PackagePath;    // Asset의 패키지 경로
    EAssetType AssetType; // Asset의 타입
    uint32 Size;          // Asset의 크기 (바이트 단위)

    [[nodiscard]] FString GetFullPath() const { return PackagePath.ToString() / AssetName.ToString(); }
};

struct FAssetRegistry
{
    TMap<FName, FAssetInfo> PathNameToAssetInfo;
};

struct FFbxLoadResult
{
    TArray<USkeleton*> Skeletons;
    TArray<USkeletalMesh*> SkeletalMeshes;
    TArray<UStaticMesh*> StaticMeshes;
    TArray<UMaterial*> Materials;
    TArray<UAnimationAsset*> Animations;
};

class UAssetManager : public UObject
{
    DECLARE_CLASS(UAssetManager, UObject)

private:
    std::unique_ptr<FAssetRegistry> AssetRegistry;

public:
    UAssetManager() = default;
    virtual ~UAssetManager() override;

    static bool IsInitialized();

    /** UAssetManager를 가져옵니다. */
    static UAssetManager& Get();

    /** UAssetManager가 존재하면 가져오고, 없으면 nullptr를 반환합니다. */
    static UAssetManager* GetIfInitialized();
    
    void InitAssetManager();

    const TMap<FName, FAssetInfo>& GetAssetRegistry();
    TMap<FName, FAssetInfo>& GetAssetRegistryRef();

    USkeletalMesh* GetSkeletalMesh(const FName& Name);
    UStaticMesh* GetStaticMesh(const FName& Name);
    USkeleton* GetSkeleton(const FName& Name);
    UMaterial* GetMaterial(const FName& Name);
    UAnimationAsset* GetAnimation(const FName& Name);

    void AddAssetInfo(const FAssetInfo& Info);
    void AddSkeleton(const FName& Key, USkeleton* Skeleton);
    void AddSkeletalMesh(const FName& Key, USkeletalMesh* SkeletalMesh);
    void AddMaterial(const FName& Key, UMaterial* Material);
    void AddStaticMesh(const FName& Key, UStaticMesh* StaticMesh);
    void AddAnimation(const FName& Key, UAnimationAsset* Animation);

private:
    void LoadContentFiles();

    inline static TMap<FName, USkeleton*> SkeletonMap;
    inline static TMap<FName, USkeletalMesh*> SkeletalMeshMap;
    inline static TMap<FName, UStaticMesh*> StaticMeshMap;
    inline static TMap<FName, UMaterial*> MaterialMap;
    inline static TMap<FName, UAnimationAsset*> AnimationMap;
};
