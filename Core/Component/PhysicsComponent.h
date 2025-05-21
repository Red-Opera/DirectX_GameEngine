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

    void SetGravity(bool enable);
    bool IsGravityEnabled() const { return useGravity; }
    void SetMass(float mass);
    float GetMass() const { return mass; }
    
    
    void UpdateColliderSize();              // 콜라이더 크기 업데이트 메서드

    virtual std::string GetClassName() const override;
    static std::string GetStaticClassName() { return "PhysicsComponent"; }

    physx::PxRigidActor* GetActor() const { return actor; }

private:
    float mass = 1.0f;
    bool isDynamic = true;
    bool useGravity = true;
    
    physx::PxRigidActor* actor = nullptr;

    // Transform 변경 감지용 이전 값 저장
    Position lastTransformPosition;
    Position lastTransformRotation;
    Position lastTransformScale;
};