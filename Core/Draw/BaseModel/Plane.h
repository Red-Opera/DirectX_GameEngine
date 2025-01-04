#pragma once

#include <array>
#include <optional>
#include <DirectXMath.h>

#include "Core/RenderingPipeline/Vertex.h"
#include "Core/Draw/Base/TriangleIndexList.h"

#include "Utility/MathInfo.h"

class Plane
{
public:
	static TriangleIndexList MakeTesslatedTextured(VertexCore::VertexLayout vertexLayout, int divisionX, int divisionY)
	{
		assert(divisionX >= 1);
		assert(divisionY >= 1);

		constexpr float width = 1.0f;
		constexpr float height = 1.0f;

		const int verticesX = divisionX + 1;
		const int verticesY = divisionY + 1;

		VertexCore::VertexBuffer vertexBuffer{ std::move(vertexLayout) };

		const float sizeX = width / 2.0f;
		const float sizeY = height / 2.0f;

		const float divisionSizeX = width / float(divisionX);
		const float divisionSizeY = height / float(divisionY);

		const float divisionXTc = 1.0f / float(divisionX);
		const float divisionYTc = 1.0f / float(divisionY);

		for (int y = 0, i = 0; y < verticesY; y++)
		{
			const float yPos = float(y) * divisionY - 1.0f;
			const float yPosTc = 1.0f - float(y) * divisionSizeY - 1.0f;

			for (int x = 0; x < verticesX; x++, i++) 
			{
				const float xPos = float(x) * divisionSizeX - 1.0f;
				const float xPosTc = float(x) * divisionXTc;

				vertexBuffer.emplace_back(
					DirectX::XMFLOAT3{ xPos, yPos,0.0f },
					DirectX::XMFLOAT3{ 0.0f, 0.0f, -1.0f },
					DirectX::XMFLOAT2{ xPosTc, yPosTc });
			}
		}

		std::vector<USHORT> indices;
		indices.reserve(pow(divisionX * divisionSizeY, 2) * 6);

		const auto vxy2i = [verticesX](size_t x, size_t y) { return (USHORT)(y * verticesX + x); };

		for (size_t y = 0; y < divisionY; y++)
		{
			for (size_t x = 0; x < divisionX; x++)
			{
				const std::array<USHORT, 4> indexArray = { vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1), vxy2i(x + 1, y + 1) };

				indices.push_back(indexArray[0]);
				indices.push_back(indexArray[2]);
				indices.push_back(indexArray[1]);
				indices.push_back(indexArray[1]);
				indices.push_back(indexArray[2]);
				indices.push_back(indexArray[3]);
			}
		}
		return { std::move(vertexBuffer), std::move(indices) };
	}

	static TriangleIndexList Make()
	{
		using VertexCore::VertexLayout;

		VertexLayout vertexLaout;
		vertexLaout.AddType(VertexLayout::Position3D);
		vertexLaout.AddType(VertexLayout::Normal);
		vertexLaout.AddType(VertexLayout::Texture2D);

		return MakeTesslatedTextured(std::move(vertexLaout), 1, 1);
	}
};