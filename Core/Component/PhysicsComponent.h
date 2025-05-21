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

    virtual std::string GetClassName() const override;
    static std::string GetStaticClassName() { return "PhysicsComponent"; }

    physx::PxRigidActor* GetActor() const { return actor; }

private:
    physx::PxRigidActor* actor = nullptr;
    float mass = 1.0f;
    bool isDynamic = true;
    bool useGravity = true;
};