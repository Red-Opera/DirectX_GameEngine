#pragma once

#include <optional>
#include <DirectXMath.h>

#include "../Base/TriangleIndexList.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Utility/MathInfo.h"

class Sphere
{
public:
	static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int latDiv, int longDiv)
	{
		namespace dx = DirectX;
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float lattitudeAngle = Math::PI / latDiv, longitudeAngle = 2.0f * Math::PI / longDiv;

		VertexCore::VertexBuffer vertices(std::move(vertexLayout));
		for (int iLat = 1; iLat < latDiv; iLat++)
		{
			const auto latBase = dx::XMVector3Transform(base, dx::XMMatrixRotationX(lattitudeAngle * iLat));

			for (int iLong = 0; iLong < longDiv; iLong++)
			{
				DirectX::XMFLOAT3 calculatedPos;

				auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longitudeAngle * iLong));
				dx::XMStoreFloat3(&calculatedPos, v);

				vertices.emplace_back(calculatedPos);
			}
		}

		// add the cap vertices
		const auto iNorthPole = (unsigned short)vertices.size();
		dx::XMFLOAT3 northPos;
		dx::XMStoreFloat3(&northPos, base);
		vertices.emplace_back(northPos);

		const auto iSouthPole = (unsigned short)vertices.size();
		dx::XMFLOAT3 southPos;
		dx::XMStoreFloat3(&southPos, dx::XMVectorNegate(base));
		vertices.emplace_back(southPos);

		const auto calcIdx = [latDiv, longDiv] (unsigned short iLat, unsigned short iLong) { return iLat * longDiv + iLong; };

		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}

			// wrap band
			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		// cap fans
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			// north
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));

			// south
			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(iSouthPole);
		}

		// wrap triangles
		// north
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(calcIdx(0, 0));

		// south
		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return { std::move(vertices), std::move(indices) };
	}

	static TriangleIndexList Make(std::optional<VertexCore::VertexLayout> vertexLayout = std::nullopt)
	{
		using VertexType = VertexCore::VertexLayout::VertexType;

		if (!vertexLayout)
			vertexLayout = VertexCore::VertexLayout{}.AddType(VertexType::Position3D);

		return MakeTesselated(std::move(*vertexLayout), 12, 24);
	}
};