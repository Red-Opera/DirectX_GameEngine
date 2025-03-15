#pragma once

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"
#include "Utility/MathInfo.h"

#include <DirectXMath.h>
#include <array>
#include <optional>

class CubeFrame
{
public:
	static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
	static TriangleIndexList CreateIndependentFrame(VertexCore::VertexLayout vertexLayout);
	static TriangleIndexList CreateTextureFrame();
};

