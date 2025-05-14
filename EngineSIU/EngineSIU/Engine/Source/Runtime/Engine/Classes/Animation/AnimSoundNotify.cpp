#include "AnimSoundNotify.h"
#include "SoundManager.h"

UAnimSoundNotify::UAnimSoundNotify()
{
    FSoundManager::GetInstance().LoadSound("footprint","Contents/Sounds/footprint.mp3");
}

void UAnimSoundNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    FSoundManager::GetInstance().PlaySound("footprint");
}

