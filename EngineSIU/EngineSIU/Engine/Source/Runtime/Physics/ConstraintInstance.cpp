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

    //TransformInA = ConstraintSetup->TransformInA;
    //TransformInB = ConstraintSetup->TransformInB;

    PxPhysics* Phys = UPhysicsManager::Get().GetPhysics();
    //PxRigidActor* ActorA = BodyInstanceA->GetActor()->rigidBody;
    //PxRigidActor* ActorB = BodyInstanceB->GetActor()->rigidBody;

    //PxRigidDynamic* ParentBody = static_cast<PxRigidDynamic*>(ActorA);
    //PxRigidDynamic* ChildBody = static_cast<PxRigidDynamic*>(ActorB);

    //// 1) 부모·자식 본의 월드 Pose를 가져온다
    //PxTransform parentPose = ParentBody->getGlobalPose();
    //PxTransform childPose = ChildBody->getGlobalPose();

    //// 2) 앵커 위치를 원하는 대로 정한다. 예: 두 본의 중간점
    //PxVec3 worldAnchorPos = (childPose.p) * 0.5f;
    //PxQuat worldAnchorRot = PxQuat(PxIdentity);

    //PxTransform worldAnchor(worldAnchorPos, worldAnchorRot);

    //PxTransform frameA = parentPose.transformInv(worldAnchor);
    //PxTransform frameB = childPose.transformInv(worldAnchor);

    //PxTransform localFrameParent = PxTransform(ParentBody->getGlobalPose().getInverse() * PxTransform(ChildPos));
    //PxTransform FrameB = PxTransform(PxVec3(0));

    //FrameA = FTransform::Identity.ToPxTransform();
    //FrameB = FTransform::Identity.ToPxTransform();

    //D6Joint = PxD6JointCreate(*Phys, ParentBody, frameA, ChildBody, frameB);

    //D6Joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);

    BodyInstanceA = OwnerComponent->GetBodyInstance(ConstraintSetup->BoneNameA);
    BodyInstanceB = OwnerComponent->GetBodyInstance(ConstraintSetup->BoneNameB);
    //JointName = Setup->JointName;
    PxRigidActor* PActor1 = BodyInstanceA->GetActor()->rigidBody;
    PxRigidActor* PActor2 = BodyInstanceB->GetActor()->rigidBody;
    PxTransform parentActorWorldPose = PActor1->getGlobalPose();
    PxTransform childActorWorldPose = PActor2->getGlobalPose();
    PxQuat q_AxisCorrection = PxQuat(PxMat33(
        PxVec3(0.0f, 0.0f, 1.0f), // new X-axis column
        PxVec3(1.0f, 0.0f, 0.0f), // new Y-axis column
        PxVec3(0.0f, 1.0f, 0.0f)  // new Z-axis column
    ));
    q_AxisCorrection.normalize();
    PxTransform PLocalFrame_Child(PxVec3(0.0f), q_AxisCorrection);
    PxTransform childJointFrameInWorld = childActorWorldPose * PLocalFrame_Child;
    PxTransform PLocalFrame_Parent = parentActorWorldPose.getInverse() * childJointFrameInWorld;
    PxD6Joint* D6Joint = PxD6JointCreate(*Phys,
        PActor1,            // actor0 = 자식 (PActor2)
        PLocalFrame_Parent,  // localFrame0 = 수정된 자식 기준 프레임
        PActor2,            // actor1 = 부모 (PActor1)
        PLocalFrame_Child  // localFrame1 = 수정된 부모 기준 프레임
    );

    if (!D6Joint)
    {
        // 로그: 조인트 생성 실패
        return;
    }

    D6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
    D6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
    D6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

    D6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);
    D6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
    D6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);

    D6Joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);

    D6Joint->setTwistLimit(PxJointAngularLimitPair(-PxPi / 4, PxPi / 4));
    D6Joint->setSwingLimit(PxJointLimitCone(PxPi / 6, PxPi / 6));

    D6Joint->setProjectionLinearTolerance(0.5f);
    D6Joint->setProjectionAngularTolerance(FMath::DegreesToRadians(1.0f));
    D6Joint->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
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
