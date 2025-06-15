#include "stdafx.h"
#include "PhysicsSystem.h"

using namespace physx;

// =================================
//	Singleton Pattern
// =================================

PhysicsSystem& PhysicsSystem::GetInstance()
{
    static PhysicsSystem instance;

    return instance;
}

// =================================
//	System Lifecycle
// =================================

bool PhysicsSystem::Initialize()
{
    // PhysX 기반 시스템 초기화
    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
    
    if (foundation == nullptr)
        return false;

    // PVD 연결 설정 (PhysX Visual Debugger)
    pvd = PxCreatePvd(*foundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

    if (pvd != nullptr)
        pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    
    // PhysX 초기화
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
    if (physics == nullptr)
        return false;
    
    // 물리 장면 생성
    PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);  // 중력 설정
    dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD; // CCD 씬 플래그 활성화
    sceneDesc.ccdMaxPasses = 4; // CCD 최대 패스 수 (충돌 정확도 향상)
    
    scene = physics->createScene(sceneDesc);
    
    return true;
}

void PhysicsSystem::Shutdown()
{
    // 리소스 정리
    if (scene != nullptr)
    {
        scene->release();
        scene = nullptr;
    }

    if (dispatcher != nullptr)
    {
        dispatcher->release();
        dispatcher = nullptr;
    }

    if (physics != nullptr)
    {
        physics->release();
        physics = nullptr;
    }

    if (pvd != nullptr)
    {
        PxPvdTransport* transport = pvd->getTransport();
        pvd->release();
        pvd = nullptr;

        if (transport != nullptr)
            transport->release();
    }

    if (foundation != nullptr)
    {
        foundation->release();
        foundation = nullptr;
    }
}

void PhysicsSystem::Update(float deltaTime)
{
    if (scene == nullptr)
        return;

    // 물리 시뮬레이션 진행 (안정적인 서브스테핑 방식)
    const float fixedStepSize = 1.0f / 60.0f;   // 60Hz 고정 스텝
    const int maxSubSteps = 5;                  // 최대 서브스텝 수 (성능 보호)
    
    timeAccumulator += deltaTime;
    
    int subSteps = 0;

    while (timeAccumulator >= fixedStepSize && subSteps < maxSubSteps)
    {
        scene->simulate(fixedStepSize);
        scene->fetchResults(true);
        
        timeAccumulator -= fixedStepSize;
        subSteps++;
    }
    
    // 누적 시간이 너무 크면 리셋 (스파이럴 오브 데스 방지)
    if (timeAccumulator > fixedStepSize * maxSubSteps)
        timeAccumulator = 0.0f;
}

// =================================
//	Mesh Collider Creation
// =================================

// 삼각형 메시 콜라이더 생성 (정적 오브젝트용)
PxTriangleMesh* PhysicsSystem::CreateTriangleMeshCollider(
    const std::vector<PxVec3>& vertices,
    const std::vector<uint32_t>& indices)
{
    if (physics == nullptr || vertices.empty() || indices.empty())
        return nullptr;
        
    PxCookingParams cookingParams(physics->getTolerancesScale());
    cookingParams.meshWeldTolerance = 0.001f;
    cookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eWELD_VERTICES;
        
    // 메시 설명 생성
    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = static_cast<uint32_t>(vertices.size());
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices.data();
    
    meshDesc.triangles.count = static_cast<uint32_t>(indices.size()) / 3;
    meshDesc.triangles.stride = 3 * sizeof(uint32_t);
    meshDesc.triangles.data = indices.data();
    
    // 메모리 스트림 생성
    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;
    
    // 메시 요리(쿠킹)
    bool cookingStatus = PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer, &result);

    if (!cookingStatus)
        return nullptr;
    
    // 메모리 입력 스트림 생성
    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    
    // 삼각형 메시 생성
    return physics->createTriangleMesh(readBuffer);
}

// 컨벡스 메시 콜라이더 생성 (동적 오브젝트용)
PxConvexMesh* PhysicsSystem::CreateConvexMeshCollider(
    const std::vector<PxVec3>& vertices,
    bool autoGenerateHull,
    uint32_t vertexLimit)
{
    if (physics == nullptr || vertices.empty())
        return nullptr;
        
    // Cooking 파라미터 설정
    PxCookingParams cookingParams(physics->getTolerancesScale());
    cookingParams.meshWeldTolerance = 0.001f;
    cookingParams.meshPreprocessParams = PxMeshPreprocessingFlag::eWELD_VERTICES;
        
    // 컨벡스 메시 설명 생성
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = static_cast<uint32_t>(vertices.size());
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data = vertices.data();
    
    // 자동 헐(hull) 생성 옵션 설정
    if (autoGenerateHull)
    {
        convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
        
        // 정점 제한 설정 (성능 최적화)
        if (vertexLimit > 0 && vertexLimit < 256)
            convexDesc.vertexLimit = vertexLimit;
    }
    
    // 메모리 스트림 생성
    PxDefaultMemoryOutputStream writeBuffer;
    PxConvexMeshCookingResult::Enum result;
    
    // 메시 요리(쿠킹)
    bool cookingStatus = PxCookConvexMesh(cookingParams, convexDesc, writeBuffer, &result);

    if (!cookingStatus)
        return nullptr;
    
    // 메모리 입력 스트림 생성
    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    
    // 컨벡스 메시 생성
    return physics->createConvexMesh(readBuffer);
}

// =================================
//	Destructor
// =================================

PhysicsSystem::~PhysicsSystem()
{
    Shutdown();
}