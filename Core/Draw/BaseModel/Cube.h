#pragma once

#include "../Base/TriangleIndexList.h"

#include "Core/RenderingPipeline/Vertex.h"
#include "Utility/MathInfo.h"

#include <DirectXMath.h>
#include <array>
#include <optional>

class Cube
{
public:
	static TriangleIndexList Create(std::optional<VertexCore::VertexLayout> vertexLayout = { })
	{
		using namespace VertexCore;
		using VertexType = VertexCore::VertexLayout::VertexType;

		if (!vertexLayout)
		{
			vertexLayout = VertexCore::VertexLayout{ };
			vertexLayout->AddType(VertexType::Position3D);
		}

		constexpr float side = 1.0f / 2.0f;

		VertexBuffer vertices(std::move(*vertexLayout), 8u);
		vertices[0].GetValue<VertexType::Position3D>() = { -side, -side, -side };
		vertices[1].GetValue<VertexType::Position3D>() = { side, -side, -side };
		vertices[2].GetValue<VertexType::Position3D>() = { -side,  side, -side };
		vertices[3].GetValue<VertexType::Position3D>() = { side,  side, -side };
		vertices[4].GetValue<VertexType::Position3D>() = { -side, -side,  side };
		vertices[5].GetValue<VertexType::Position3D>() = { side, -side,  side };
		vertices[6].GetValue<VertexType::Position3D>() = { -side,  side,  side };
		vertices[7].GetValue<VertexType::Position3D>() = { side,  side,  side };

		return 
		{
			std::move(vertices),
			{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}

	static TriangleIndexList MakeCubeFrame(VertexCore::VertexLayout vertexLayout)
	{
		using namespace VertexCore;
		using VertexType = VertexCore::VertexLayout::VertexType;

		constexpr float side = 1.0f / 2.0f;

		VertexBuffer vertices(std::move(vertexLayout), 24u);
		vertices[0].GetValue<VertexType::Position3D>() = { -side, -side, -side };
		vertices[1].GetValue<VertexType::Position3D>() = {  side, -side, -side };
		vertices[2].GetValue<VertexType::Position3D>() = { -side,  side, -side };
		vertices[3].GetValue<VertexType::Position3D>() = {  side,  side, -side };
		vertices[4].GetValue<VertexType::Position3D>() = { -side, -side,  side };
		vertices[5].GetValue<VertexType::Position3D>() = {  side, -side,  side };
		vertices[6].GetValue<VertexType::Position3D>() = { -side,  side,  side };
		vertices[7].GetValue<VertexType::Position3D>() = {  side,  side,  side };
		vertices[8].GetValue<VertexType::Position3D>() = { -side, -side, -side };
		vertices[9].GetValue<VertexType::Position3D>() = { -side,  side, -side };
		vertices[10].GetValue<VertexType::Position3D>() = { -side, -side,  side };
		vertices[11].GetValue<VertexType::Position3D>() = { -side,  side,  side };
		vertices[12].GetValue<VertexType::Position3D>() = {  side, -side, -side };
		vertices[13].GetValue<VertexType::Position3D>() = {  side,  side, -side };
		vertices[14].GetValue<VertexType::Position3D>() = {  side, -side,  side };
		vertices[15].GetValue<VertexType::Position3D>() = {  side,  side,  side };
		vertices[16].GetValue<VertexType::Position3D>() = { -side, -side, -side };
		vertices[17].GetValue<VertexType::Position3D>() = {  side, -side, -side };
		vertices[18].GetValue<VertexType::Position3D>() = { -side, -side,  side };
		vertices[19].GetValue<VertexType::Position3D>() = {  side, -side,  side };
		vertices[20].GetValue<VertexType::Position3D>() = { -side,  side, -side };
		vertices[21].GetValue<VertexType::Position3D>() = {  side,  side, -side };
		vertices[22].GetValue<VertexType::Position3D>() = { -side,  side,  side };
		vertices[23].GetValue<VertexType::Position3D>() = {  side,  side,  side };

		return {
			std::move(vertices),{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
			}
		};
	}

	static TriangleIndexList SetCubeTexturePosition()
	{
		using namespace VertexCore;
		using VertexType = VertexCore::VertexLayout::VertexType;

		auto textureVertices = MakeCubeFrame(std::move(VertexLayout{}
			.AddType(VertexType::Position3D)
			.AddType(VertexType::Normal)
			.AddType(VertexType::Texture2D)
		));

		textureVertices.vertices[0].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
		textureVertices.vertices[1].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
		textureVertices.vertices[2].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
		textureVertices.vertices[3].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
		textureVertices.vertices[4].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
		textureVertices.vertices[5].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
		textureVertices.vertices[6].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
		textureVertices.vertices[7].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
		textureVertices.vertices[8].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
		textureVertices.vertices[9].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
		textureVertices.vertices[10].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
		textureVertices.vertices[11].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
		textureVertices.vertices[12].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
		textureVertices.vertices[13].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
		textureVertices.vertices[14].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
		textureVertices.vertices[15].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
		textureVertices.vertices[16].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
		textureVertices.vertices[17].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
		textureVertices.vertices[18].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
		textureVertices.vertices[19].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
		textureVertices.vertices[20].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
		textureVertices.vertices[21].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
		textureVertices.vertices[22].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
		textureVertices.vertices[23].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };

		return textureVertices;
	}
	
	static TriangleIndexList CreateTextureCube() { return SetCubeTexturePosition(); }
};

