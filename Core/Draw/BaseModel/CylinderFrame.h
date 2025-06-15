#pragma once

#include"IVertexProvider.h"

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"

#include <optional>
#include <vector>

class CylinderFrame : public IVertexProvider
{
public:
	static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int divisionCount);

	static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = { });
	static TriangleIndexList CreateTextureFrame();

	// PhysX 콜라이더용 정점 데이터 생성
	static bool GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
		std::vector<uint32_t>& indices,
		const Scale& scale);
};