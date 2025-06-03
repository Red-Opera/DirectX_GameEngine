#pragma once

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"
#include "Utility/MathInfo.h"

#include <DirectXMath.h>
#include <array>
#include <optional>

/**
 * @brief 3D 큐브 모델의 메시를 생성하는 유틸리티 클래스
 * 
 * 다양한 렌더링 요구사항에 맞는 큐브 메시를 생성하는 정적 메서드들을 제공합니다.
 * 기본 큐브, 독립적인 버텍스를 가진 큐브, 텍스처 매핑용 큐브 등을 생성할 수 있습니다.
 */
class CubeFrame
{
public:
    // 기본 큐브 프레임을 생성
    static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
    
    // 독립적인 버텍스를 가진 큐브 프레임을 생성
    static TriangleIndexList CreateIndependentFrame(VertexCore::VertexLayout vertexLayout);

    // 텍스처 매핑을 위한 큐브 프레임을 생성
    static TriangleIndexList CreateTextureFrame();
};

