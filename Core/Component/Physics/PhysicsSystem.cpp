#include "stdafx.h"
#include "PhysicsSystem.h"

PhysicsSystem& PhysicsSystem::GetInstance()
{
    static PhysicsSystem instance;

    return instance;
}

bool PhysicsSystem::Initialize()
{
    // PhysX 기반 시스템 초기화
    foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback);
    
    if (foundation == nullptr)
        return false;
    
    // PVD 연결 설정 (PhysX Visual Debugger)
    pvd = PxCreatePvd(*foundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

    if (pvd != nullptr)
        pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
    
    // PhysX 초기화
    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, pvd);
    if (physics == nullptr)
        return false;
    
    // 물리 장면 생성
    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);  // 중력 설정
    dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    scene = physics->createScene(sceneDesc);
    
    return true;
}

void PhysicsSystem::Shutdown()
{
    // 리소스 정리
    if (scene != nullptr)
        scene->release();

    if (dispatcher != nullptr)
        dispatcher->release();

    if (physics != nullptr)
        physics->release();

    if (pvd != nullptr)
    {
        physx::PxPvdTransport* transport = pvd->getTransport();
        pvd->release();

        if (transport != nullptr)
            transport->release();
    }

    if (foundation != nullptr)
        foundation->release();
    
    scene = nullptr;
    dispatcher = nullptr;
    physics = nullptr;
    pvd = nullptr;
    foundation = nullptr;
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

PhysicsSystem::~PhysicsSystem()
{
    Shutdown();
}