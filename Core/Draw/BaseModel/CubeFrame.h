#pragma once

#include "../Base/TriangleIndexList.h"
#include "IVertexProvider.h"

#include "Core/RenderingPipeline/Vertex.h"
#include "Utility/MathInfo.h"

#include <DirectXMath.h>
#include <array>
#include <optional>

/**
 * @brief 3D 큐브 모델의 메시를 생성하는 유틸리티 클래스
 */
class CubeFrame : public IVertexProvider
{
public:
    // 기본 큐브 프레임을 생성
    static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
    
    // 독립적인 버텍스를 가진 큐브 프레임을 생성
    static TriangleIndexList CreateIndependentFrame(VertexCore::VertexLayout vertexLayout);

    // 텍스처 매핑을 위한 큐브 프레임을 생성
    static TriangleIndexList CreateTextureFrame();
    
    // PhysX 콜라이더용 정점 데이터 생성
    static bool GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
                               std::vector<uint32_t>& indices,
                               const Scale& scale);
};

