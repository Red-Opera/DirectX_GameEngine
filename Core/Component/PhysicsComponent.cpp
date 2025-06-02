#include "stdafx.h"
#include "PhysicsComponent.h"

#include "Core/Component/Transform/Transform.h"
#include "Core/Object/Object.h"
#include "Physics/PhysicsSystem.h"

// 전역 중력 기본값 설정 (지구 중력)
physx::PxVec3 PhysicsComponent::globalGravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

PhysicsComponent::PhysicsComponent(std::shared_ptr<class Object> object, float mass, bool isDynamic)
    : Component(object), mass(mass), isDynamic(isDynamic)
{
}

PhysicsComponent::~PhysicsComponent()
{
    // PhysX 액터 리소스 해제
    if (actor == nullptr)
        return;

    actor->release();
    actor = nullptr;
}

void PhysicsComponent::Initialize()
{
    // PhysX 시스템에서 필요한 객체들 가져오기
    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxScene* scene = PhysicsSystem::GetInstance().GetScene();

    // 현재 Transform의 위치와 회전 가져오기
    Position position = transform->GetPosition();
    Quaternion rotation = transform->GetRotation();

    // PhysX Transform 생성 (쿼터니언 사용)
    physx::PxTransform pxTransform(
        physx::PxVec3(position.x, position.y, position.z),
        ConvertToPhysXQuaternion(rotation)
    );

    // 액터 생성
    if (isDynamic)
    {
        // 동적 물체 생성 (중력의 영향을 받고 이동 가능)
        physx::PxRigidDynamic* dynamicActor = physics->createRigidDynamic(pxTransform);

        // 중력 설정 적용 (사용자 정의 중력 스케일이 있으면 기본 중력 비활성화)
        if (gravityScale != 1.0f && useGravity)
            dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

        // 일반 중력 설정
        else
            dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);

        // 운동학적 객체 설정
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);

        // 충돌 감지 모드 설정
        SetCollisionDetectionMode(collisionMode);

        // 질량 설정 및 관성 모멘트 자동 계산
        physx::PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, mass);

        // 공기 저항 설정 (기본 + 사용자 정의 드래그)
        dynamicActor->setLinearDamping(linearDamping + dragCoefficient);
        dynamicActor->setAngularDamping(angularDamping + (dragCoefficient * 0.5f));

        actor = dynamicActor;
    }

    else
    {
        // 정적 물체 생성 (움직이지 않는 고정 물체)
        physx::PxRigidStatic* staticActor = physics->createRigidStatic(pxTransform);

        actor = staticActor;
    }

    // 기본 박스 형태의 충돌 형상 생성
    physx::PxShape* shape = physics->createShape
    (
        physx::PxBoxGeometry(0.5f, 0.5f, 0.5f),
        *physics->createMaterial(staticFriction, dynamicFriction, restitution)
    );

    // 액터에 충돌 형상 연결
    actor->attachShape(*shape);
    shape->release();

    // 물리 시뮬레이션 씬에 액터 추가 (여기로 이동)
    if (scene != nullptr)
        scene->addActor(*actor);

    // 제약 조건 적용 (씬에 추가한 후에 호출)
    UpdateConstraints();

    // 초기 Transform 값 저장
    lastTransform = transform->GetTransform();
}

void PhysicsComponent::OnEnable()
{
    if (actor == nullptr)
        return;

    // 물리 시뮬레이션 활성화
    actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);

    // 동적 물체인 경우 추가 설정
    if (isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

        // 중력 설정 복원 (사용자 정의 중력 스케일이 있으면 기본 중력 비활성화)
        if (gravityScale != 1.0f && useGravity)
            dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

        else if (useGravity)
            dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);

        else
            dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

        // 운동학적 객체 설정 복원
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);

        // 제약 조건 복원
        UpdateConstraints();

        // 잠자고 있는 액터를 깨우기
        dynamicActor->wakeUp();
    }

    // 액터가 씬에 있는지 확인하고 없으면 추가
    physx::PxScene* scene = PhysicsSystem::GetInstance().GetScene();

    if (scene)
    {
        // 현재 씬에 있는 모든 액터 가져오기
        physx::PxU32 numActors = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);
        physx::PxActor** actors = new physx::PxActor * [numActors];

        scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, actors, numActors);

        // 현재 액터가 씬에 있는지 확인
        bool isInScene = false;

        for (physx::PxU32 i = 0; i < numActors; i++)
        {
            if (actors[i] == nullptr)
                continue;

            isInScene = true;
            break;
        }

        delete[] actors;

        // 씬에 없으면 추가
        if (!isInScene)
            scene->addActor(*actor);
    }
}

void PhysicsComponent::OnDisable()
{
    if (actor == nullptr)
        return;

    // 동적 물체인 경우 속도를 먼저 초기화 (시뮬레이션 비활성화 전)
    if (isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

        dynamicActor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
        dynamicActor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
    }

    // 물리 시뮬레이션 비활성화
    actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
}

void PhysicsComponent::Update()
{
    if (actor == nullptr)
        return;

    // 컴포넌트 활성화 상태와 PhysX 액터 시뮬레이션 상태 동기화
    bool isSimulationDisabled = actor->getActorFlags() & physx::PxActorFlag::eDISABLE_SIMULATION;
    bool shouldBeDisabled = !GetEnable();

    if (isSimulationDisabled != shouldBeDisabled)
    {
        if (shouldBeDisabled)
        {
            // 비활성화: 먼저 속도 초기화 후 시뮬레이션 비활성화
            if (isDynamic)
            {
                physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

                dynamicActor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
                dynamicActor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
            }

            actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
        }

        // 활성화: 시뮬레이션 활성화
        else
            actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
    }

    // 컴포넌트가 비활성화되어 있으면 물리 업데이트 중단
    if (!GetEnable())
        return;

    Position position = transform->GetPosition();
    Quaternion rotation = transform->GetRotation();

    if (isDynamic)
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

        // 사용자 정의 중력 적용 (필요한 경우)
        if (useGravity && gravityScale != 1.0f && !isKinematic)
            ApplyCustomGravity();

        // 키네마틱 모드인 경우 Transform에서 PhysX로 동기화만 수행
        if (isKinematic)
        {
            // Transform에서 PhysX로 위치와 회전 동기화
            physx::PxTransform pxTransform(
                physx::PxVec3(position.x, position.y, position.z),
                ConvertToPhysXQuaternion(rotation)
            );

            dynamicActor->setKinematicTarget(pxTransform);

            // 최신 Transform 값 저장
            lastTransform = transform->GetTransform();
        }

        else
        {
            // Inspector나 외부에서 Transform이 변경되었는지 확인
            if (lastTransform != transform->GetTransform())
            {
                // Transform에서 PhysX로 위치와 회전 동기화 (외부 변경 반영)
                physx::PxTransform pxTransform(
                    physx::PxVec3(position.x, position.y, position.z),
                    ConvertToPhysXQuaternion(rotation)
                );

                dynamicActor->setGlobalPose(pxTransform);

                // 스케일이 변경된 경우 콜라이더 크기도 업데이트
                if (lastTransform.GetScale() != transform->GetScale())
                    UpdateColliderSize();

                // 최신 Transform 값 저장
                lastTransform = transform->GetTransform();

                // 사용자가 직접 이동한 경우 속도를 0으로 초기화하여 자연스럽게 정지
                dynamicActor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
                dynamicActor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));

                // 잠자고 있는 액터를 깨우기
                dynamicActor->wakeUp();
            }

            else
            {
                // PhysX에서 Transform으로 위치와 회전 동기화 (물리 시뮬레이션 결과 반영)
                physx::PxTransform pose = dynamicActor->getGlobalPose();

                // 물리 시뮬레이션 결과를 Transform에 적용
                transform->SetPosition(pose.p.x, pose.p.y, pose.p.z);

                // 회전도 업데이트 (PhysX 쿼터니언을 Transform 쿼터니언으로 변환)
                Quaternion newRotation = ConvertFromPhysXQuaternion(pose.q);
                transform->SetRotation(newRotation);

                // 최신 Transform 값 업데이트
                lastTransform = transform->GetTransform();
            }
        }
    }

    else
    {
        // 정적 물체의 경우 Transform에서 PhysX로만 동기화 (물리 영향 없음)
        if (lastTransform != transform->GetTransform())
        {
            physx::PxRigidStatic* staticActor = static_cast<physx::PxRigidStatic*>(actor);

            physx::PxTransform pxTransform(
                physx::PxVec3(position.x, position.y, position.z),
                ConvertToPhysXQuaternion(rotation)
            );

            staticActor->setGlobalPose(pxTransform);

            // 스케일 변경 시 콜라이더 크기 업데이트
            if (lastTransform.GetScale() != transform->GetScale())
                UpdateColliderSize();

            // 최신 값 저장
            lastTransform = transform->GetTransform();
        }
    }
}

// 사용자 정의 중력 적용
void PhysicsComponent::ApplyCustomGravity()
{
    if (!isDynamic || isKinematic || !useGravity || actor == nullptr)
        return;

    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 사용자 중력 스케일 적용
    physx::PxVec3 scaledGravity = globalGravity * gravityScale;

    // 중력을 힘으로 적용 (가속도 모드)
    dynamicActor->addForce(scaledGravity, physx::PxForceMode::eACCELERATION);
}

void PhysicsComponent::Finalize()
{
    // 컴포넌트 종료 시 필요한 정리 작업
}

void PhysicsComponent::SetGravity(bool enable)
{
    useGravity = enable;

    if (actor == nullptr || !isDynamic)
        return;

    // 동적 물체인 경우 중력 설정 즉시 적용
    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    if (gravityScale != 1.0f && enable)
    {
        // 사용자 정의 중력 스케일이 있으면 기본 중력 비활성화
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    }
    else
    {
        // 일반 중력 설정
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
    }

    // 물체를 명시적으로 깨우기
    dynamicActor->wakeUp();
}

// 새로 추가: 중력 배율 설정
void PhysicsComponent::SetGravityScale(float scale)
{
    // 이전 값과 같으면 변경하지 않음
    if (gravityScale == scale)
        return;

    gravityScale = scale;

    if (actor == nullptr || !isDynamic || !useGravity || isKinematic)
        return;

    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 표준 중력으로 돌아가는 경우 기본 중력 사용
    if (gravityScale == 1.0f)
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);

    // 사용자 정의 중력을 적용하는 경우 기본 중력 비활성화
    else
        dynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

    // 액터 깨우기
    dynamicActor->wakeUp();
}

// 새로 추가: 키네마틱 설정
void PhysicsComponent::SetKinematic(bool kinematic)
{
    if (isKinematic == kinematic)
        return;

    isKinematic = kinematic;

    if (actor == nullptr || !isDynamic)
        return;

    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 키네마틱 상태 변경
    dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);

    // 키네마틱으로 변경 시, 속도 초기화
    if (isKinematic)
    {
        dynamicActor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
        dynamicActor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));

        // 현재 위치와 회전으로 키네마틱 타겟 설정
        Position position = transform->GetPosition();
        Quaternion rotation = transform->GetRotation();
        physx::PxTransform pxTransform(
            physx::PxVec3(position.x, position.y, position.z),
            ConvertToPhysXQuaternion(rotation)
        );
        dynamicActor->setKinematicTarget(pxTransform);
    }

    // 액터 깨우기
    dynamicActor->wakeUp();
}

// 새로 추가: 충돌 감지 모드 설정
void PhysicsComponent::SetCollisionDetectionMode(CollisionDetectionMode mode)
{
    collisionMode = mode;

    if (actor == nullptr || !isDynamic)
        return;

    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    switch (collisionMode)
    {
    case CollisionDetectionMode::Discrete:
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, false);
        break;

    case CollisionDetectionMode::Continuous:
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION, false);
        break;

    case CollisionDetectionMode::ContinuousDynamic:
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
        dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD_FRICTION, true);
        break;
    }
}

// 새로 추가: 드래그 계수 설정
void PhysicsComponent::SetDragCoefficient(float coefficient)
{
    dragCoefficient = coefficient;

    if (actor == nullptr || !isDynamic)
        return;

    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 기본 감쇠 + 드래그 계수 적용
    float effectiveLinearDamping = linearDamping + dragCoefficient;
    float effectiveAngularDamping = angularDamping + (dragCoefficient * 0.5f);

    dynamicActor->setLinearDamping(effectiveLinearDamping);
    dynamicActor->setAngularDamping(effectiveAngularDamping);
}

void PhysicsComponent::SetGlobalGravity(float x, float y, float z)
{
    globalGravity = physx::PxVec3(x, y, z);

    // 현재 물리 씬의 중력 설정 업데이트
    physx::PxScene* scene = PhysicsSystem::GetInstance().GetScene();

    if (scene == nullptr)
        return;

    scene->setGravity(globalGravity);

    // 씬의 모든 동적 물체 깨우기
    physx::PxU32 numActors = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);

    if (numActors == 0)
        return;

    std::vector<physx::PxRigidDynamic*> actors(numActors);
    scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, reinterpret_cast<physx::PxActor**>(&actors[0]), numActors);

    for (physx::PxU32 i = 0; i < numActors; i++)
    {
        if (actors[i] == nullptr && !actors[i]->isSleeping())
            continue;

        actors[i]->wakeUp();
    }
}

physx::PxVec3 PhysicsComponent::GetGlobalGravity()
{
    return globalGravity;
}

void PhysicsComponent::SetMass(float newMass)
{
    mass = newMass;

    if (actor == nullptr || !isDynamic || isKinematic)
        return;

    // 동적 물체인 경우 질량 설정 즉시 적용
    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
    physx::PxRigidBodyExt::setMassAndUpdateInertia(*dynamicActor, mass);
}

void PhysicsComponent::SetMaterial(float staticFric, float dynamicFric, float rest)
{
    staticFriction = staticFric;
    dynamicFriction = dynamicFric;
    restitution = rest;

    if (actor == nullptr)
        return;

    // 액터에 연결된 모든 Shape의 재질 속성 업데이트
    uint32_t numShapes = actor->getNbShapes();
    physx::PxShape* shapes[8];  // 최대 8개의 Shape 지원
    actor->getShapes(shapes, numShapes);

    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxMaterial* newMaterial = physics->createMaterial(staticFriction, dynamicFriction, restitution);

    // 모든 Shape에 새로운 재질 적용
    for (uint32_t i = 0; i < numShapes; i++)
        shapes[i]->setMaterials(&newMaterial, 1);

    // 재질 해제
    newMaterial->release();
}

void PhysicsComponent::SetLinearDamping(float damping)
{
    linearDamping = damping;

    if (actor == nullptr || !isDynamic)
        return;

    // 동적 물체인 경우 선형 공기 저항 즉시 적용
    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 드래그 계수를 포함한 최종 감쇠 계산
    float effectiveDamping = linearDamping + dragCoefficient;
    dynamicActor->setLinearDamping(effectiveDamping);
}

void PhysicsComponent::SetAngularDamping(float damping)
{
    angularDamping = damping;

    if (actor == nullptr || !isDynamic)
        return;

    // 동적 물체인 경우 각도 공기 저항 즉시 적용
    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 드래그 계수를 포함한 최종 감쇠 계산
    float effectiveDamping = angularDamping + (dragCoefficient * 0.5f);
    dynamicActor->setAngularDamping(effectiveDamping);
}

void PhysicsComponent::SetConstraints(uint32_t constraintFlags)
{
    // 이전 상태와 동일한지 확인
    if (constraints == constraintFlags)
        return;

    // 제약 조건 플래그 업데이트
    constraints = constraintFlags;

    // 제약 조건 즉시 적용
    UpdateConstraints();

    // 모든 제약 조건이 해제되었고 액터가 씬에 있다면 깨우기
    if (actor != nullptr && isDynamic && constraints == 0 && IsActorInScene())
    {
        physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);
        dynamicActor->wakeUp();

        // 중력이 꺼져있을 경우 작은 임펄스를 줘서 움직임 시작
        if (!useGravity)
            dynamicActor->addForce(physx::PxVec3(0.0f, 0.1f, 0.0f), physx::PxForceMode::eIMPULSE);
    }
}

void PhysicsComponent::AddConstraints(uint32_t constraintFlags)
{
    // 이미 설정된 플래그를 추가하면 변화 없음
    if ((constraints & constraintFlags) == constraintFlags)
        return;

    // 기존 플래그에 새 플래그 추가
    SetConstraints(constraints | constraintFlags);
}

void PhysicsComponent::RemoveConstraints(uint32_t constraintFlags)
{
    // 제거할 플래그가 설정되어 있지 않으면 변화 없음
    if ((constraints & constraintFlags) == 0)
        return;

    // 기존 플래그에서 지정된 플래그 제거
    SetConstraints(constraints & ~constraintFlags);
}

void PhysicsComponent::SetFreezePosition(bool x, bool y, bool z)
{
    // 임시 변수에 현재 회전 제약 조건 저장 (유지해야 함)
    uint32_t rotationConstraints = constraints & FREEZE_ROTATION;

    // 새로운 위치 제약 조건 계산
    uint32_t positionConstraints = 0;
    if (x)
        positionConstraints |= FREEZE_POSITION_X;

    if (y)
        positionConstraints |= FREEZE_POSITION_Y;

    if (z)
        positionConstraints |= FREEZE_POSITION_Z;

    // 전체 제약 조건 업데이트 (회전 제약 조건은 유지)
    SetConstraints(rotationConstraints | positionConstraints);
}

void PhysicsComponent::SetFreezeRotation(bool x, bool y, bool z)
{
    // 임시 변수에 현재 위치 제약 조건 저장 (유지해야 함)
    uint32_t positionConstraints = constraints & FREEZE_POSITION;

    // 새로운 회전 제약 조건 계산
    uint32_t rotationConstraints = 0;

    if (x)
        rotationConstraints |= FREEZE_ROTATION_X;

    if (y)
        rotationConstraints |= FREEZE_ROTATION_Y;

    if (z)
        rotationConstraints |= FREEZE_ROTATION_Z;

    // 전체 제약 조건 업데이트 (위치 제약 조건은 유지)
    SetConstraints(positionConstraints | rotationConstraints);
}

void PhysicsComponent::UpdateConstraints()
{
    // 정적 물체는 제약 조건이 필요하지 않음
    if (actor == nullptr || !isDynamic)
        return;

    physx::PxRigidDynamic* dynamicActor = static_cast<physx::PxRigidDynamic*>(actor);

    // 제약 조건 플래그 초기화
    physx::PxRigidDynamicLockFlags lockFlags = physx::PxRigidDynamicLockFlag::Enum(0);

    // 위치 제약 조건 설정
    if (constraints & FREEZE_POSITION_X)
        lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;

    if (constraints & FREEZE_POSITION_Y)
        lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;

    if (constraints & FREEZE_POSITION_Z)
        lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

    // 회전 제약 조건 설정
    if (constraints & FREEZE_ROTATION_X)
        lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;

    if (constraints & FREEZE_ROTATION_Y)
        lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;

    if (constraints & FREEZE_ROTATION_Z)
        lockFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

    // 제약 조건 액터에 적용
    dynamicActor->setRigidDynamicLockFlags(lockFlags);

    // 액터가 씬에 있는 경우에만 wakeUp 호출
    if (IsActorInScene())
        dynamicActor->wakeUp();
}

// 충돌 이벤트 처리 메서드 구현
void PhysicsComponent::OnCollisionEnter(const physx::PxContactPair& contactPair)
{
    // 여기에 충돌 시작 시 처리할 로직 구현
}

void PhysicsComponent::OnCollisionStay(const physx::PxContactPair& contactPair)
{
    // 여기에 충돌 중 처리할 로직 구현
}

void PhysicsComponent::OnCollisionExit(const physx::PxContactPair& contactPair)
{
    // 여기에 충돌 종료 시 처리할 로직 구현
}

std::string PhysicsComponent::GetClassName() const
{
    return GetStaticClassName();
}

void PhysicsComponent::UpdateColliderSize()
{
    // 기존 충돌 형상 제거
    uint32_t numShapes = actor->getNbShapes();
    physx::PxShape* shapes[8];  // 최대 8개의 Shape 지원

    actor->getShapes(shapes, numShapes);

    // 모든 Shape 분리
    for (uint32_t i = 0; i < numShapes; i++)
        actor->detachShape(*shapes[i]);

    // Transform 스케일에 맞는 새로운 크기로 Shape 생성
    // PhysX는 half-extent를 사용하므로 스케일 값을 2로 나눔
    physx::PxPhysics* physics = PhysicsSystem::GetInstance().GetPhysics();
    physx::PxShape* shape = physics->createShape
    (
        physx::PxBoxGeometry
        (
            transform->GetScale().x * 0.5f,  // 박스 폭의 절반
            transform->GetScale().y * 0.5f,  // 박스 높이의 절반
            transform->GetScale().z * 0.5f   // 박스 깊이의 절반
        ),
        *physics->createMaterial(staticFriction, dynamicFriction, restitution)
    );

    // 새로운 Shape를 액터에 연결
    actor->attachShape(*shape);
    shape->release();
}

// PhysicsComponent 클래스에 액터가 씬에 있는지 확인하는 헬퍼 함수 추가
bool PhysicsComponent::IsActorInScene() const
{
    if (actor == nullptr)
        return false;

    // PhysX 액터가 씬에 연결되어 있는지 확인
    return actor->getScene() != nullptr;
}

// 쿼터니언 변환 헬퍼 함수들 구현
physx::PxQuat PhysicsComponent::ConvertToPhysXQuaternion(const Quaternion& quat) const
{
    // DirectX 쿼터니언을 PhysX 쿼터니언으로 변환
    return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}

Quaternion PhysicsComponent::ConvertFromPhysXQuaternion(const physx::PxQuat& pxQuat) const
{
    // PhysX 쿼터니언을 DirectX 쿼터니언으로 변환
    return Quaternion{ pxQuat.x, pxQuat.y, pxQuat.z, pxQuat.w };
}