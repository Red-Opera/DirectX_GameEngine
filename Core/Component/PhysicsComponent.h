#pragma once

#include "Core/Component/Component.h"
#include "External/physx/physX/include/PxPhysicsAPI.h"

#include "Core/Component/Transform/Transform.h"

// BaseModel Frame 헤더들
#include "Core/Draw/BaseModel/CubeFrame.h"
#include "Core/Draw/BaseModel/SphereFrame.h"
#include "Core/Draw/BaseModel/ConeFrame.h"
#include "Core/Draw/BaseModel/CylinderFrame.h"
#include "Core/Draw/BaseModel/ColorPlaneFrame.h"

// 모든 Color Object 헤더 추가
#include "Core/Draw/Object/ColorCubeObject.h"
#include "Core/Draw/Object/ColorSphereObject.h"
#include "Core/Draw/Object/ColorConeObject.h"
#include "Core/Draw/Object/ColorCylinderObject.h"
#include "Core/Draw/Object/ColorPlaneObject.h"
#include "Core/Draw/Object/TextureCubeObject.h"

// =================================
//	Enumerations
// =================================

// 충돌 감지 모드를 위한 열거형
enum class CollisionDetectionMode
{
    Discrete,           // 기본 충돌 감지 방식
    Continuous,         // 빠르게 움직이는 물체를 위한 고급 충돌 감지
    ContinuousDynamic   // 빠르게 움직이는 물체 간 충돌을 위한 고급 충돌 감지
};

// 콜라이더 타입 열거형 추가
enum class ColliderType
{
    Box,            // 박스 콜라이더
    Sphere,         // 구 콜라이더
    Capsule,        // 캡슐 콜라이더
    TriangleMesh,   // 삼각형 메시 (정적 객체용)
    ConvexMesh      // 컨벡스 메시 (동적 객체용)
};

// 축 고정 플래그를 위한 열거형
enum ConstraintFlags
{
    // 위치 고정 플래그
    FREEZE_POSITION_X = (1 << 0),
    FREEZE_POSITION_Y = (1 << 1),
    FREEZE_POSITION_Z = (1 << 2),

    // 회전 고정 플래그
    FREEZE_ROTATION_X = (1 << 3),
    FREEZE_ROTATION_Y = (1 << 4),
    FREEZE_ROTATION_Z = (1 << 5),

    // 편의를 위한 조합 플래그
    FREEZE_POSITION = (FREEZE_POSITION_X | FREEZE_POSITION_Y | FREEZE_POSITION_Z),
    FREEZE_ROTATION = (FREEZE_ROTATION_X | FREEZE_ROTATION_Y | FREEZE_ROTATION_Z),
    FREEZE_ALL = (FREEZE_POSITION | FREEZE_ROTATION)
};

class PhysicsComponent : public Component
{
public:
    // =================================
    //	Constructor & Destructor
    // =================================

    PhysicsComponent(std::shared_ptr<class Object> object, float mass = 1.0f, bool isDynamic = true);
    virtual ~PhysicsComponent() override;

    // =================================
    //	Component Override Methods
    // =================================

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;
    virtual void Finalize() override;
    virtual std::string GetClassName() const override;
    static std::string GetStaticClassName() { return "PhysicsComponent"; }

    // 컴포넌트 활성화/비활성화 처리
    virtual void OnEnable() override;
    virtual void OnDisable() override;

    // =================================
    //	Collider Management
    // =================================

    // 콜라이더 타입 설정
    void SetColliderType(ColliderType type);
    ColliderType GetColliderType() const { return colliderType; }

    // 메시 기반 콜라이더 생성
    void CreateMeshColliderFromModel(bool useConvex = true);
    void CreateCustomMeshCollider(const std::vector<physx::PxVec3>& vertices, 
                                  const std::vector<uint32_t>& indices, 
                                  bool useConvex = true);

    // 콜라이더 크기와 제약 조건 업데이트
    void UpdateColliderSize();
    void UpdateConstraints();

    // =================================
    //	Physics Properties
    // =================================

    // 질량 설정
    void SetMass(float mass);
    float GetMass() const { return mass; }

    // 운동학적 객체 설정
    void SetKinematic(bool kinematic);
    bool IsKinematic() const { return isKinematic; }

    // 충돌 감지 모드 설정
    void SetCollisionDetectionMode(CollisionDetectionMode mode);
    CollisionDetectionMode GetCollisionDetectionMode() const { return collisionMode; }

    // =================================
    //	Gravity Management
    // =================================

    // 중력 관련 설정
    void SetGravity(bool enable);
    bool IsGravityEnabled() const { return useGravity; }
    static void SetGlobalGravity(float x, float y, float z);
    static physx::PxVec3 GetGlobalGravity();

    // 사용자 중력 배율 설정
    void SetGravityScale(float scale);
    float GetGravityScale() const { return gravityScale; }

    // =================================
    //	Material Properties
    // =================================

    // 물질 속성 설정 (마찰력, 반발력)
    void SetMaterial(float staticFriction, float dynamicFriction, float restitution);
    float GetStaticFriction() const { return staticFriction; }
    float GetDynamicFriction() const { return dynamicFriction; }
    float GetRestitution() const { return restitution; }

    // =================================
    //	Damping & Drag
    // =================================

    // 공기 저항 설정
    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    float GetLinearDamping() const { return linearDamping; }
    float GetAngularDamping() const { return angularDamping; }

    // 사용자 정의 공기 저항 계수
    void SetDragCoefficient(float coefficient);
    float GetDragCoefficient() const { return dragCoefficient; }

    // =================================
    //	Constraints Management
    // =================================

    // 제약 조건 설정 (비트 마스크 사용)
    void SetConstraints(uint32_t constraintFlags);
    void AddConstraints(uint32_t constraintFlags);
    void RemoveConstraints(uint32_t constraintFlags);
    uint32_t GetConstraints() const { return constraints; }

    // 위치 / 회전 제약 조건 설정 (기존 인터페이스 유지)
    void SetFreezePosition(bool x, bool y, bool z);
    void SetFreezeRotation(bool x, bool y, bool z);

    // 개별 축 제약 조건 확인 (비트 연산으로 구현)
    bool IsPositionXFrozen() const { return (constraints & FREEZE_POSITION_X) != 0; }
    bool IsPositionYFrozen() const { return (constraints & FREEZE_POSITION_Y) != 0; }
    bool IsPositionZFrozen() const { return (constraints & FREEZE_POSITION_Z) != 0; }
    bool IsRotationXFrozen() const { return (constraints & FREEZE_ROTATION_X) != 0; }
    bool IsRotationYFrozen() const { return (constraints & FREEZE_ROTATION_Y) != 0; }
    bool IsRotationZFrozen() const { return (constraints & FREEZE_ROTATION_Z) != 0; }

    // =================================
    //	Collision Events
    // =================================

    // 충돌 이벤트 처리 메서드 추가
    void OnCollisionEnter(const physx::PxContactPair& contactPair);
    void OnCollisionStay(const physx::PxContactPair& contactPair);
    void OnCollisionExit(const physx::PxContactPair& contactPair);

    // =================================
    //	Physics Access
    // =================================

    physx::PxRigidActor* GetActor() const { return actor; }

private:
    // =================================
    //	Collider Creation Helpers
    // =================================

    // 콜라이더 생성 헬퍼 함수들
    void CreateBoxCollider();
    void CreateSphereCollider();
    void CreateCapsuleCollider();
    void CreateConeCollider();
    physx::PxShape* CreateShapeFromType();

    // 정점 데이터 추출 헬퍼 함수
    bool ExtractVerticesFromModel(std::vector<physx::PxVec3>& vertices, 
                                  std::vector<uint32_t>& indices);

    // =================================
    //	Internal Physics Methods
    // =================================

    // 사용자 정의 중력 적용을 위한 내부 메서드
    void ApplyCustomGravity();

    // 액터가 씬에 있는지 확인하는 헬퍼 함수
    bool IsActorInScene() const;

    // 쿼터니언 변환 헬퍼 함수들
    physx::PxQuat ConvertToPhysXQuaternion(const Quaternion& quat) const;
    Quaternion ConvertFromPhysXQuaternion(const physx::PxQuat& pxQuat) const;

    // =================================
    //	Physics Properties
    // =================================

    float mass = 1.0f;              // 물체의 질량
    bool isDynamic = true;          // 동적 물체 여부
    bool useGravity = true;         // 중력 적용 여부

    // 콜라이더 타입
    ColliderType colliderType = ColliderType::Box;

    // 새로운 물리 속성들
    bool isKinematic = false;       // 운동학적 객체 여부
    float gravityScale = 1.0f;      // 사용자 중력 배율
    float dragCoefficient = 0.0f;   // 사용자 정의 공기 저항 계수
    CollisionDetectionMode collisionMode = CollisionDetectionMode::Discrete; // 충돌 감지 모드

    // =================================
    //	Material Properties
    // =================================

    // 물질 속성
    float staticFriction = 0.5f;    // 정적 마찰력
    float dynamicFriction = 0.5f;   // 동적 마찰력
    float restitution = 0.6f;       // 반발력

    // 공기 저항
    float linearDamping = 0.0f;     // 선형 공기 저항
    float angularDamping = 0.05f;   // 각도 공기 저항

    // =================================
    //	Constraints & Global Settings
    // =================================

    // 제약 조건 (비트 마스크 사용)
    uint32_t constraints = 0;       // 모든 축 제약 조건을 비트로 관리

    // 전역 중력 설정
    static physx::PxVec3 globalGravity;

    // =================================
    //	PhysX Integration
    // =================================

    // PhysX 액터 객체
    physx::PxRigidActor* actor = nullptr;

    // Transform 변경 감지를 위한 이전 값 저장
    Transform lastTransform;
};