#pragma once

#include <PxPhysicsAPI.h>

class PhysicsSystem
{
public:
    static PhysicsSystem& GetInstance();
    
    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);
    
    physx::PxPhysics* GetPhysics() const { return physics; }
    physx::PxScene* GetScene() const { return scene; }

private:
    PhysicsSystem() = default;
    ~PhysicsSystem();
    
    // PhysX 기본 객체들
    physx::PxDefaultAllocator allocator;
    physx::PxDefaultErrorCallback errorCallback;
    physx::PxFoundation* foundation = nullptr;
    physx::PxPhysics* physics = nullptr;
    physx::PxDefaultCpuDispatcher* dispatcher = nullptr;
    physx::PxScene* scene = nullptr;
    physx::PxPvd* pvd = nullptr;
};