#include "stdafx.h"
#include "PhysicsComponent.h"
#include "Physics/PhysicsSystem.h"

#include "Core/Object/Object.h"

// 전역 중력 초기화
physx::PxVec3 PhysicsComponent::globalGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

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
    // PhysX 액터 생성 로직
    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxScene* scene = PhysicsSystem::GetInstance().GetScene();

    Position position = transform->GetPosition();
    
    if (isDynamic)
    {
        // 동적 객체 생성
        physx::PxRigidDynamic* dynamicActor = physics->createRigidDynamic(
            physx::PxTransform(physx::PxVec3(position.x, position.y, position.z))
        );
        
        // 중력 설정
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
        
        // 질량 설정
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, mass);
        
        // 공기 저항 설정
        dynamicActor->setLinearDamping(linearDamping);
        dynamicActor->setAngularDamping(angularDamping);
        
        actor = dynamicActor;
    }
    else
    {
        // 정적 객체 생성
        physx::PxRigidStatic* staticActor = physics->createRigidStatic(
            physx::PxTransform(physx::PxVec3(position.x, position.y, position.z))
        );
        
        actor = staticActor;
    }
    
    // 기본 박스 모양 콜라이더 추가
    physx::PxShape* shape = physics->createShape(
        physx::PxBoxGeometry(0.5f, 0.5f, 0.5f),
        *physics->createMaterial(staticFriction, dynamicFriction, restitution)
    );
    
    actor->attachShape(*shape);
    shape->release();
    
    // 제약 조건 적용
    UpdateConstraints();
    
    // 물리 씬에 액터 추가
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
        
        // �ν����ͳ� ����𿡼� ����� Ʈ������ ���� �����Ͽ� ���� ��ü�� ����
        // lastTransformPosition�� ����� Transform�� �ܺο��� ����Ǿ����� Ȯ��
        if (lastTransformPosition != transform->GetPosition() ||
            lastTransformRotation != transform->GetRotation() ||
            lastTransformScale != transform->GetScale())
        {
            // Transform -> PhysX ����ȭ
            physx::PxTransform pxTransform(
                physx::PxVec3(position.x, position.y, position.z)
            );
            
            // ȸ�� ���ʹϾ����� ��ȯ (���� �ʿ�)
            // pxTransform.q = ConvertToQuaternion(transform->GetRotation());
            
            dynamicActor->setGlobalPose(pxTransform);
            
            // ������ ���� �� �ݶ��̴� ũ�� ����
            if (lastTransformScale != transform->GetScale())
            {
                UpdateColliderSize();
            }
            
            // �ֱ� Transform �� ����
            lastTransformPosition = transform->GetPosition();
            lastTransformRotation = transform->GetRotation();
            lastTransformScale = transform->GetScale();
            
            // ����ڰ� ���� ������ ��� �ӵ��� 0���� �����Ͽ� �� �ڿ������� ����
            dynamicActor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
            dynamicActor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
        }

        else
        {
            // PhysX -> Transform ����ȭ
            physx::PxTransform pose = dynamicActor->getGlobalPose();
            
            // ���� ���������� ��ġ�� Transform�� ����
            transform->SetPosition(pose.p.x, pose.p.y, pose.p.z);
            
            // ȸ���� ������Ʈ (���ʹϾ𿡼� ���Ϸ������� ��ȯ �ʿ�)
            // transform->SetRotation(...);
            
            // �ֱ� Transform �� ������Ʈ
            lastTransformPosition = transform->GetPosition();
            lastTransformRotation = transform->GetRotation();
        }
    }

    else
    {
        // ���� ��ü�� ��� Transform -> PhysX�� ����ȭ (�� ����)
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
    // �ʿ��� ���� �۾� ����
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

void PhysicsComponent::SetGlobalGravity(float x, float y, float z)
{
    globalGravity = physx::PxVec3(x, y, z);
    
    // 모든 씬의 중력 업데이트
    physx::PxScene* scene = PhysicsSystem::GetInstance().GetScene();
    if (scene)
    {
        scene->setGravity(globalGravity);
    }
}

physx::PxVec3 PhysicsComponent::GetGlobalGravity()
{
    return globalGravity;
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

void PhysicsComponent::SetMaterial(float staticFric, float dynamicFric, float rest)
{
    staticFriction = staticFric;
    dynamicFriction = dynamicFric;
    restitution = rest;
    
    if (actor)
    {
        // 기존 Shape들의 재질 업데이트
        uint32_t numShapes = actor->getNbShapes();
        physx::PxShape* shapes[8];
        actor->getShapes(shapes, numShapes);
        
        physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
        physx::PxMaterial* newMaterial = physics->createMaterial(staticFriction, dynamicFriction, restitution);
        
        for (uint32_t i = 0; i < numShapes; i++)
        {
            shapes[i]->setMaterials(&newMaterial, 1);
        }
    }
}

void PhysicsComponent::SetLinearDamping(float damping)
{
    linearDamping = damping;
    
    if (actor && isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        dynamicActor->setLinearDamping(linearDamping);
    }
}

void PhysicsComponent::SetAngularDamping(float damping)
{
    angularDamping = damping;
    
    if (actor && isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        dynamicActor->setAngularDamping(angularDamping);
    }
}

void PhysicsComponent::SetFreezePosition(bool x, bool y, bool z)
{
    freezePositionX = x;
    freezePositionY = y;
    freezePositionZ = z;
    
    UpdateConstraints();
}

void PhysicsComponent::SetFreezeRotation(bool x, bool y, bool z)
{
    freezeRotationX = x;
    freezeRotationY = y;
    freezeRotationZ = z;
    
    UpdateConstraints();
}

void PhysicsComponent::UpdateConstraints()
{
    if (!actor || !isDynamic)
        return;
    
    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
    
    physx::PxRigidDynamicLockFlags lockFlags = physx::PxRigidDynamicLockFlag::Enum(0);
    
    if (freezePositionX) lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
    if (freezePositionY) lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
    if (freezePositionZ) lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
    if (freezeRotationX) lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
    if (freezeRotationY) lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
    if (freezeRotationZ) lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
    
    dynamicActor->setRigidDynamicLockFlags(lockFlags);
}

std::string PhysicsComponent::GetClassName() const
{
    return GetStaticClassName();
}

void PhysicsComponent::UpdateColliderSize()
{
    // ���� Shape ����
    uint32_t numShapes = actor->getNbShapes();
    physx::PxShape* shapes[8];                  // �ִ� 8�� Shape ����
    actor->getShapes(shapes, numShapes);
    
    for (uint32_t i = 0; i < numShapes; i++)
    {
        actor->detachShape(*shapes[i]);
    }
    
    // �� ũ��� Shape ���� (half-extent ������ ���� ������ ���� 2�� ����)
    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxShape* shape = physics->createShape(
        physx::PxBoxGeometry(
            transform->GetScale().x * 0.5f,  // ���� ũ��� ����
            transform->GetScale().y * 0.5f,  // ���� ũ��� ����
            transform->GetScale().z * 0.5f   // ���� ũ��� ����
        ),
        *physics->createMaterial(0.5f, 0.5f, 0.6f)
    );
    
    actor->attachShape(*shape);
    shape->release();
}