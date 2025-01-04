#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Core/Exception/WindowException.h"
#include "Core/RenderingPipeline/Vertex.h"

class TriangleIndexList
{
public:
	TriangleIndexList() = default;
	TriangleIndexList(VertexCore::VertexBuffer vertices, std::vector<unsigned short> indices) : vertices(std::move(vertices)), indices(std::move(indices))
	{
		assert("삼각형의 정점 개수가 부족합니다." && this->vertices.size() > 2);
		assert("삼각형의 인덱스가 맞지 않습니다." && this->indices.size() % 3 == 0);
	}

	void Transform(DirectX::XMMATRIX matrix)
	{
		using VertexType = VertexCore::VertexLayout::VertexType;

		for (int i = 0; i < vertices.count(); i++)
		{
			auto& pos = vertices[i].GetValue<VertexType::Position3D>();

			DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix));
		}
	}

	// Normal Vector를 구하고 저장하는 메소드
	void SetNormalVector() NOEXCEPTRELEASE
	{
		using namespace DirectX;
		using VertexType = VertexCore::VertexLayout::VertexType;

		// 오브젝트의 모든 면에 대해서 Normal Vector를 구함
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			// 삼각형의 각 정점을 가져옴
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];
	
			// 정점의 위치를 가져옴
			const auto pos0 = DirectX::XMLoadFloat3(&v0.GetValue<VertexType::Position3D>());
			const auto pos1 = DirectX::XMLoadFloat3(&v1.GetValue<VertexType::Position3D>());
			const auto pos2 = DirectX::XMLoadFloat3(&v2.GetValue<VertexType::Position3D>());
	
			// Normal Vector를 구함 (삼각형의 외적을 통해 구함)
			const auto normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(pos1, pos0), DirectX::XMVectorSubtract(pos2, pos0)));
	
			// 정점의 Normal Vector를 돌려줌
			XMStoreFloat3(&v0.GetValue<VertexType::Normal>(), normal);
			XMStoreFloat3(&v1.GetValue<VertexType::Normal>(), normal);
			XMStoreFloat3(&v2.GetValue<VertexType::Normal>(), normal);
		}
	}

	VertexCore::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};