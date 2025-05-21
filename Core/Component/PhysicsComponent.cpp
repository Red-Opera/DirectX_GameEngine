#include "stdafx.h"
#include "PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"

#include "Core/Object/Object.h"

PhysicsComponent::PhysicsComponent(std::shared_ptr<class Object> object, float mass, bool isDynamic)
    : Component(object), mass(mass), isDynamic(isDynamic)
{
}

PhysicsComponent::~PhysicsComponent()
{
    if (actor == nullptr)
        return;

    actor->release();
    actor = nullptr;
}

void PhysicsComponent::Initialize()
{
    // PhysX 물리 액터 생성
    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxScene* scene = PhysicsSystem::GetInstance().GetScene();
    
    if (isDynamic)
    {
        // 동적 강체 생성
        physx::PxRigidDynamic* dynamicActor = physics->createRigidDynamic(
            physx::PxTransform(
                physx::PxVec3(
                    transform->GetPosition().x,
                    transform->GetPosition().y,
                    transform->GetPosition().z
                )
            )
        );
        
        // 중력 설정
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
        
        // 질량 설정
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, mass);
        
        actor = dynamicActor;
    }

    else
    {
        // 정적 강체 생성
        physx::PxRigidStatic* staticActor = physics->createRigidStatic(
            physx::PxTransform(
                physx::PxVec3(
                    transform->GetPosition().x,
                    transform->GetPosition().y,
                    transform->GetPosition().z
                )
            )
        );
        
        actor = staticActor;
    }
    
    // 기본 박스 형태 콜라이더 추가
    physx::PxShape* shape = physics->createShape(
        physx::PxBoxGeometry(1.0f, 1.0f, 1.0f),
        *physics->createMaterial(0.5f, 0.5f, 0.6f)
    );
    
    actor->attachShape(*shape);
    shape->release();
    
    // 물리 세계에 액터 추가
    scene->addActor(*actor);
}

void PhysicsComponent::Update()
{
    if (isDynamic && actor)
    {
        // 물리 시뮬레이션에서 위치와 회전을 가져와 오브젝트 트랜스폼 업데이트
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        physx::PxTransform pose = dynamicActor->getGlobalPose();
        
        transform->SetPosition(pose.p.x, pose.p.y, pose.p.z);
        
        // 회전도 업데이트 (쿼터니언을 오일러 각으로 변환 필요)
        // ... 회전 업데이트 코드
    }
}

void PhysicsComponent::Finalize()
{
    // 필요한 정리 작업 수행
}

void PhysicsComponent::SetGravity(bool enable)
{
    useGravity = enable;
    
    if (actor && isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
    }
}

void PhysicsComponent::SetMass(float newMass)
{
    mass = newMass;
    
    if (actor && isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, mass);
    }
}

std::string PhysicsComponent::GetClassName() const
{
    return GetStaticClassName();
}