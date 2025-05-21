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

    Position position = transform->GetPosition();
    
    if (isDynamic)
    {
        // 동적 강체 생성
        physx::PxRigidDynamic* dynamicActor = physics->createRigidDynamic(
            physx::PxTransform(physx::PxVec3(position.x,position.y, position.z))
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
                physx::PxVec3(position.x, position.y, position.z)
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
    if (actor == nullptr)
        return;

	Position position = transform->GetPosition();
        
    if (isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        
        // 인스펙터나 기즈모에서 변경된 트랜스폼 값을 감지하여 물리 객체에 적용
        // lastTransformPosition을 사용해 Transform이 외부에서 변경되었는지 확인
        if (lastTransformPosition != transform->GetPosition() ||
            lastTransformRotation != transform->GetRotation() ||
            lastTransformScale != transform->GetScale())
        {
            // Transform -> PhysX 동기화
            physx::PxTransform pxTransform(
                physx::PxVec3(position.x, position.y, position.z)
            );
            
            // 회전 쿼터니언으로 변환 (구현 필요)
            // pxTransform.q = ConvertToQuaternion(transform->GetRotation());
            
            dynamicActor->setGlobalPose(pxTransform);
            
            // 스케일 변경 시 콜라이더 크기 조정
            if (lastTransformScale != transform->GetScale())
            {
                UpdateColliderSize();
            }
            
            // 최근 Transform 값 저장
            lastTransformPosition = transform->GetPosition();
            lastTransformRotation = transform->GetRotation();
            lastTransformScale = transform->GetScale();
            
            // 사용자가 직접 움직인 경우 속도를 0으로 리셋하여 더 자연스럽게 만듦
            dynamicActor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
            dynamicActor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
        }

        else
        {
            // PhysX -> Transform 동기화
            physx::PxTransform pose = dynamicActor->getGlobalPose();
            
            // 물리 엔진에서의 위치를 Transform에 적용
            transform->SetPosition(pose.p.x, pose.p.y, pose.p.z);
            
            // 회전도 업데이트 (쿼터니언에서 오일러각으로 변환 필요)
            // transform->SetRotation(...);
            
            // 최근 Transform 값 업데이트
            lastTransformPosition = transform->GetPosition();
            lastTransformRotation = transform->GetRotation();
        }
    }

    else
    {
        // 정적 객체인 경우 Transform -> PhysX만 동기화 (한 번만)
        if (lastTransformPosition != transform->GetPosition() ||
            lastTransformRotation != transform->GetRotation() ||
            lastTransformScale != transform->GetScale())
        {
            physx::PxRigidStatic* staticActor = static_cast<physx::PxRigidStatic*>(actor);
            
            physx::PxTransform pxTransform(
                physx::PxVec3(position.x, position.y, position.z)
            );
            
            staticActor->setGlobalPose(pxTransform);
            
            if (lastTransformScale != transform->GetScale())
            {
                UpdateColliderSize();
            }
            
            lastTransformPosition = transform->GetPosition();
            lastTransformRotation = transform->GetRotation();
            lastTransformScale = transform->GetScale();
        }
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

void PhysicsComponent::UpdateColliderSize()
{
    // 기존 Shape 제거
    uint32_t numShapes = actor->getNbShapes();
    physx::PxShape* shapes[8];                  // 최대 8개 Shape 가정
    actor->getShapes(shapes, numShapes);
    
    for (uint32_t i = 0; i < numShapes; i++)
    {
        actor->detachShape(*shapes[i]);
    }
    
    // 새 크기로 Shape 생성
    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxShape* shape = physics->createShape(
        physx::PxBoxGeometry(
            transform->GetScale().x,
            transform->GetScale().y,
            transform->GetScale().z
        ),
        *physics->createMaterial(0.5f, 0.5f, 0.6f)
    );
    
    actor->attachShape(*shape);
    shape->release();
}