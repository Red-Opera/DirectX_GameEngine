#pragma once

#include "Core/Draw/Base/TriangleIndexList.h"
#include "External/physx/physX/include/PxPhysicsAPI.h"
#include <vector>

/**
 * @brief 정점 데이터를 제공하는 인터페이스
 * BaseModel 클래스들이 PhysX 콜라이더용 정점 데이터를 제공할 수 있도록 함
 */
class IVertexProvider
{
public:
    /**
     * @brief PhysX 콜라이더용 정점 데이터를 추출합니다
     * @param vertices 출력될 정점 배열
     * @param indices 출력될 인덱스 배열
     * @param scale 적용할 스케일 값
     * @return 성공시 true, 실패시 false
     */
    static bool ExtractPhysXVertices(const TriangleIndexList& model,
                                   std::vector<physx::PxVec3>& vertices,
                                   std::vector<uint32_t>& indices,
                                   const Scale& scale);
};
