#pragma once
#include "IVertexProvider.h"

#include "../Base/TriangleIndexList.h"
#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"
#include "Utility/Vector.h"

#include <optional>
#include <DirectXMath.h>

class SphereFrame : public IVertexProvider
{
public:
    static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int latDiv, int longDiv);

    static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = std::nullopt);

    static TriangleIndexList CreateTextureFrame();

    // SphereFrame.h에 추가
    static bool GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
        std::vector<uint32_t>& indices,
        const Scale& scale);
};