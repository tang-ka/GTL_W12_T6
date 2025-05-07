
#include "SkeletalMeshActorTest.h"

#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/FObjLoader.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Asset/SkeletalMeshAsset.h"

ASkeletalMeshActorTest::ASkeletalMeshActorTest()
{
    SetActorTickInEditor(true);
}

void ASkeletalMeshActorTest::PostSpawnInitialize()
{
    AActor::PostSpawnInitialize();

    USceneComponent* Root = AddComponent<USceneComponent>(FName("RootComponent_0"));
    RootComponent = Root;

    MeshComp = AddComponent<USkeletalMeshComponent>(FName("SkeletalMeshComponent_0"));
    MeshComp->SetSkeletalMeshAsset(UAssetManager::Get().GetSkeletalMesh("Contents/test"));
    MeshComp->SetupAttachment(RootComponent);

    /*
    if (MeshComp->GetSkeletalMesh())
    {
        const FReferenceSkeleton& RefSkeleton = MeshComp->GetSkeletalMesh()->GetSkeleton()->GetReferenceSkeleton();
        
        for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
        {
            UStaticMeshComponent* Dot = AddComponent<UStaticMeshComponent>();
            Dot->SetStaticMesh(FObjManager::GetStaticMesh(L"Contents/SpherePrimitive.obj"));
            DotComponents.Add(Dot);
            
            int32 ParentIndex = RefSkeleton.RawRefBoneInfo[i].ParentIndex;
            if (ParentIndex != INDEX_NONE)
            {
                Dot->AttachToComponent(DotComponents[ParentIndex]);
            }
            else
            {
                Dot->AttachToComponent(RootComponent);
            }
            
            Dot->SetRelativeTransform(RefSkeleton.RawRefBonePose[i]);
            Dot->SetComponentScale3D(FVector(1.f));
        }
    }
    */
}

void ASkeletalMeshActorTest::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    /*
    ElapsedTime += DeltaTime;

    if (MeshComp->GetSkeletalMeshAsset() && MeshComp->GetSkeletalMeshAsset()->GetSkeleton() && MeshComp->AnimSequence)
    {
        for (int32 i = 0; i < RefSkeleton.RawRefBoneInfo.Num(); ++i)
        {
            DotComponents[i]->SetRelativeTransform(MeshComp->BoneTransforms[i]);
            DotComponents[i]->SetComponentScale3D(FVector(1.f));
        }
    }
    */
}
