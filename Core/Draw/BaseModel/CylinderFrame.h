#pragma once

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"

#include <optional>
#include <vector>

class CylinderFrame
{
public:
	static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int divisionCount);

	static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
	static TriangleIndexList CreateTextureFrame();
};