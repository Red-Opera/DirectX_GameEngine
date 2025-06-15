#pragma once

#include "IVertexProvider.h"

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"

#include <optional>
#include <vector>

class ConeFrame : public IVertexProvider
{
public:
	static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int longDiv);
	static TriangleIndexList MakeTesselatedSeparateBottom(VertexCore::VertexLayout vertexLayout, int longDiv);
	static TriangleIndexList MakeTesselatedIndependentFaces(int longDiv);

	static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
	static TriangleIndexList CreateTextureFrameSeparateBottom();

	// PhysX 콜라이더용 정점 데이터 생성
	static bool GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
		std::vector<uint32_t>& indices,
		const Scale& scale);
};