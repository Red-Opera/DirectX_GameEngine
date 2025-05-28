#pragma once

#include <optional>
#include <DirectXMath.h>

#include "../Base/TriangleIndexList.h"
#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"
#include "Utility/Vector.h"

class SphereFrame
{
public:
    static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int latDiv, int longDiv);

    static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = std::nullopt);

    static TriangleIndexList CreateTextureFrame();
};