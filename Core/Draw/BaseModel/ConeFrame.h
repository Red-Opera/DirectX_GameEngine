#pragma once

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"

#include <optional>
#include <vector>

class ConeFrame
{
public:
	static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int longDiv);
	static TriangleIndexList MakeTesselatedSeparateBottom(VertexCore::VertexLayout vertexLayout, int longDiv);
	static TriangleIndexList MakeTesselatedIndependentFaces(int longDiv);

	static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
	static TriangleIndexList CreateTextureFrameSeparateBottom();
};