#pragma once

#include "Core/Component/Component.h"
#include "External/physx/physX/include/PxPhysicsAPI.h"

class PhysicsComponent : public Component
{
public:
    PhysicsComponent(std::shared_ptr<class Object> object, float mass = 1.0f, bool isDynamic = true);
    virtual ~PhysicsComponent() override;

    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Finalize() override;

    // 중력 관련
    void SetGravity(bool enable);
    bool IsGravityEnabled() const { return useGravity; }
    static void SetGlobalGravity(float x, float y, float z);
    static physx::PxVec3 GetGlobalGravity();
    
    // 질량 관련
    void SetMass(float mass);
    float GetMass() const { return mass; }
    
    // 물질 속성
    void SetMaterial(float staticFriction, float dynamicFriction, float restitution);
    float GetStaticFriction() const { return staticFriction; }
    float GetDynamicFriction() const { return dynamicFriction; }
    float GetRestitution() const { return restitution; }
    
    // 공기 저항
    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    float GetLinearDamping() const { return linearDamping; }
    float GetAngularDamping() const { return angularDamping; }
    
    // 제약 조건
    void SetFreezePosition(bool x, bool y, bool z);
    void SetFreezeRotation(bool x, bool y, bool z);
    bool IsPositionXFrozen() const { return freezePositionX; }
    bool IsPositionYFrozen() const { return freezePositionY; }
    bool IsPositionZFrozen() const { return freezePositionZ; }
    bool IsRotationXFrozen() const { return freezeRotationX; }
    bool IsRotationYFrozen() const { return freezeRotationY; }
    bool IsRotationZFrozen() const { return freezeRotationZ; }
    
    void UpdateColliderSize();
    void UpdateConstraints();

    virtual std::string GetClassName() const override;
    static std::string GetStaticClassName() { return "PhysicsComponent"; }

    physx::PxRigidActor* GetActor() const { return actor; }

private:
    float mass = 1.0f;
    bool isDynamic = true;
    bool useGravity = true;
    
    // 물질 속성
    float staticFriction = 0.5f;
    float dynamicFriction = 0.5f;
    float restitution = 0.6f;
    
    // 공기 저항
    float linearDamping = 0.0f;
    float angularDamping = 0.05f;
    
    // 제약 조건
    bool freezePositionX = false;
    bool freezePositionY = false;
    bool freezePositionZ = false;
    bool freezeRotationX = false;
    bool freezeRotationY = false;
    bool freezeRotationZ = false;
    
    // 전역 중력
    static physx::PxVec3 globalGravity;
    
    physx::PxRigidActor* actor = nullptr;

    // Transform 변경 감지를 위한 값 저장
    Position lastTransformPosition;
    Position lastTransformRotation;
    Position lastTransformScale;
};