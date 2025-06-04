#pragma once

#include "External/physx/physX/include/PxPhysicsAPI.h"
#include <vector>

class PhysicsSystem
{
public:
    // =================================
    //	Singleton Pattern
    // =================================

    static PhysicsSystem& GetInstance();

    // =================================
    //	System Lifecycle
    // =================================

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // =================================
    //	Physics Access
    // =================================

    physx::PxPhysics* GetPhysics() const { return physics; }
    physx::PxScene* GetScene() const { return scene; }
    
    // =================================
    //	Mesh Collider Creation
    // =================================

    // 메시 콜라이더 생성 메서드
    physx::PxTriangleMesh* CreateTriangleMeshCollider(
        const std::vector<physx::PxVec3>& vertices,
        const std::vector<uint32_t>& indices);
        
    physx::PxConvexMesh* CreateConvexMeshCollider(
        const std::vector<physx::PxVec3>& vertices,
        bool autoGenerateHull = true,
        uint32_t vertexLimit = 256);

private:
    // =================================
    //	Constructor & Destructor
    // =================================

    PhysicsSystem() = default;
    ~PhysicsSystem();

    // =================================
    //	PhysX Core Objects
    // =================================

    // PhysX 기본 객체들
    physx::PxDefaultAllocator allocator;
    physx::PxDefaultErrorCallback errorCallback;
    physx::PxFoundation* foundation = nullptr;
    physx::PxPhysics* physics = nullptr;
    physx::PxDefaultCpuDispatcher* dispatcher = nullptr;
    physx::PxScene* scene = nullptr;
    physx::PxPvd* pvd = nullptr;

    // =================================
    //	Simulation Parameters
    // =================================

    float timeAccumulator = 0.0f;  // 시간 누적기 추가
};