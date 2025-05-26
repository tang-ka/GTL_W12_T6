#include "ConstraintInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/ConstraintSetup.h"
#include "BodyInstance.h"
#include "Engine/PhysicsManager.h"

FConstraintInstance::FConstraintInstance()
{
}

void FConstraintInstance::InitConstraint(USkeletalMeshComponent* InOwner, UConstraintSetup* Setup)
{
    OwnerComponent = InOwner;
    ConstraintSetup = Setup;
    if (!OwnerComponent || !ConstraintSetup)
    {
        return;
    }

    BodyInstanceA = OwnerComponent->GetBodyInstance(ConstraintSetup->BoneNameA);
    BodyInstanceB = OwnerComponent->GetBodyInstance(ConstraintSetup->BoneNameB);
    if (!BodyInstanceA || !BodyInstanceB)
    {
        return;
    }

    TransformInA = ConstraintSetup->TransformInA;
    TransformInB = ConstraintSetup->TransformInB;

    PxPhysics* Phys = UPhysicsManager::Get().GetPhysics();
    PxRigidActor* ActorA = BodyInstanceA->GetActor()->rigidBody;
    PxRigidActor* ActorB = BodyInstanceB->GetActor()->rigidBody;
    PxTransform FrameA = TransformInA.ToPxTransform();
    PxTransform FrameB = TransformInB.ToPxTransform();

    D6Joint = PxD6JointCreate(*Phys, ActorA, FrameA, ActorB, FrameB);

    // 축별 모션 제한 설정
    D6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
    D6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
    D6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);

    // 리밋 값 설정 (라디안 단위)
    D6Joint->setSwingLimit(PxJointLimitCone(
        FMath::DegreesToRadians(ConstraintSetup->Swing1LimitAngle),
        FMath::DegreesToRadians(ConstraintSetup->Swing1LimitAngle),
        0.0f));
    D6Joint->setTwistLimit(PxJointAngularLimitPair(
        -FMath::DegreesToRadians(ConstraintSetup->TwistLimitAngle),
        FMath::DegreesToRadians(ConstraintSetup->TwistLimitAngle)));

    // (필요시) 선형 리밋
    if (ConstraintSetup->LinearLimitSize > 0.0f)
    {
        // 허드(hard) 리밋 사용 시 PxTolerancesScale 필요
        const PxTolerancesScale& ToleranceScale = *UPhysicsManager::Get().GetTolerancesScale();
        D6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);
        D6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eLIMITED);
        D6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eLIMITED);
        PxJointLinearLimit LinearLimitHard(ToleranceScale, ConstraintSetup->LinearLimitSize);
        D6Joint->setLinearLimit(LinearLimitHard);
    }
}

void FConstraintInstance::TermConstraint()
{
    if (D6Joint)
    {
        D6Joint->release();
        D6Joint = nullptr;
    }

    OwnerComponent = nullptr;

    BodyInstanceA = nullptr;
    BodyInstanceB = nullptr;
    
    ConstraintSetup = nullptr;

    TransformInA = FTransform::Identity;
    TransformInB = FTransform::Identity;
}

void FConstraintInstance::UpdateConstraint(float DeltaTime)
{
}
