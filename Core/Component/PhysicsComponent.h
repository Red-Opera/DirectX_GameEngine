#pragma once

#include "Core/Component/Component.h"
#include "External/physx/physX/include/PxPhysicsAPI.h"

// 충돌 감지 모드를 위한 열거형 추가
enum class CollisionDetectionMode
{
    Discrete,           // 기본 충돌 감지 방식
    Continuous,         // 빠르게 움직이는 물체를 위한 고급 충돌 감지
    ContinuousDynamic   // 빠르게 움직이는 물체 간 충돌을 위한 고급 충돌 감지
};

class PhysicsComponent : public Component
{
public:
    PhysicsComponent(std::shared_ptr<class Object> object, float mass = 1.0f, bool isDynamic = true);
    virtual ~PhysicsComponent() override;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Finalize() override;

    // 중력 관련 설정
    void SetGravity(bool enable);
    bool IsGravityEnabled() const { return useGravity; }
    static void SetGlobalGravity(float x, float y, float z);
    static physx::PxVec3 GetGlobalGravity();
    
    // 사용자 중력 배율 설정
    void SetGravityScale(float scale);
    float GetGravityScale() const { return gravityScale; }
    
    // 질량 설정
    void SetMass(float mass);
    float GetMass() const { return mass; }
    
    // 운동학적 객체 설정
    void SetKinematic(bool kinematic);
    bool IsKinematic() const { return isKinematic; }
    
    // 충돌 감지 모드 설정
    void SetCollisionDetectionMode(CollisionDetectionMode mode);
    CollisionDetectionMode GetCollisionDetectionMode() const { return collisionMode; }
    
    // 물질 속성 설정 (마찰력, 반발력)
    void SetMaterial(float staticFriction, float dynamicFriction, float restitution);
    float GetStaticFriction() const { return staticFriction; }
    float GetDynamicFriction() const { return dynamicFriction; }
    float GetRestitution() const { return restitution; }
    
    // 공기 저항 설정
    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    float GetLinearDamping() const { return linearDamping; }
    float GetAngularDamping() const { return angularDamping; }
    
    // 사용자 정의 공기 저항 계수
    void SetDragCoefficient(float coefficient);
    float GetDragCoefficient() const { return dragCoefficient; }
    
    // 위치 / 회전 제약 조건 설정
    void SetFreezePosition(bool x, bool y, bool z);
    void SetFreezeRotation(bool x, bool y, bool z);
    bool IsPositionXFrozen() const { return freezePositionX; }
    bool IsPositionYFrozen() const { return freezePositionY; }
    bool IsPositionZFrozen() const { return freezePositionZ; }
    bool IsRotationXFrozen() const { return freezeRotationX; }
    bool IsRotationYFrozen() const { return freezeRotationY; }
    bool IsRotationZFrozen() const { return freezeRotationZ; }
    
    // 콜라이더 크기와 제약 조건 업데이트
    void UpdateColliderSize();
    void UpdateConstraints();

    // 충돌 이벤트 처리 메서드 추가
    void OnCollisionEnter(const physx::PxContactPair& contactPair);
    void OnCollisionStay(const physx::PxContactPair& contactPair);
    void OnCollisionExit(const physx::PxContactPair& contactPair);

    virtual std::string GetClassName() const override;
    static std::string GetStaticClassName() { return "PhysicsComponent"; }

    physx::PxRigidActor* GetActor() const { return actor; }

    // 컴포넌트 활성화/비활성화 처리
    virtual void OnEnable() override;
    virtual void OnDisable() override;

private:
    // 사용자 정의 중력 적용을 위한 내부 메서드
    void ApplyCustomGravity();

    // 액터가 씬에 있는지 확인하는 헬퍼 함수
    bool IsActorInScene() const;

    float mass = 1.0f;              // 물체의 질량
    bool isDynamic = true;          // 동적 물체 여부
    bool useGravity = true;         // 중력 적용 여부
    
    // 새로운 물리 속성들
    bool isKinematic = false;       // 운동학적 객체 여부
    float gravityScale = 1.0f;      // 사용자 중력 배율
    float dragCoefficient = 0.0f;   // 사용자 정의 공기 저항 계수
    CollisionDetectionMode collisionMode = CollisionDetectionMode::Discrete; // 충돌 감지 모드
    
    // 물질 속성
    float staticFriction = 0.5f;    // 정적 마찰력
    float dynamicFriction = 0.5f;   // 동적 마찰력
    float restitution = 0.6f;       // 반발력
    
    // 공기 저항
    float linearDamping = 0.0f;     // 선형 공기 저항
    float angularDamping = 0.05f;   // 각도 공기 저항
    
    // 제약 조건 (축별 고정 여부)
    bool freezePositionX = false;
    bool freezePositionY = false;
    bool freezePositionZ = false;
    bool freezeRotationX = false;
    bool freezeRotationY = false;
    bool freezeRotationZ = false;
    
    // 전역 중력 설정
    static physx::PxVec3 globalGravity;
    
    // PhysX 액터 객체
    physx::PxRigidActor* actor = nullptr;

    // Transform 변경 감지를 위한 이전 값 저장
    Position lastTransformPosition;
    Position lastTransformRotation;
    Position lastTransformScale;
};