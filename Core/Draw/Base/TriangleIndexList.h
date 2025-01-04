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
		assert("�ﰢ���� ���� ������ �����մϴ�." && this->vertices.size() > 2);
		assert("�ﰢ���� �ε����� ���� �ʽ��ϴ�." && this->indices.size() % 3 == 0);
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

	// Normal Vector�� ���ϰ� �����ϴ� �޼ҵ�
	void SetNormalVector() NOEXCEPTRELEASE
	{
		using namespace DirectX;
		using VertexType = VertexCore::VertexLayout::VertexType;

		// ������Ʈ�� ��� �鿡 ���ؼ� Normal Vector�� ����
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			// �ﰢ���� �� ������ ������
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];
	
			// ������ ��ġ�� ������
			const auto pos0 = DirectX::XMLoadFloat3(&v0.GetValue<VertexType::Position3D>());
			const auto pos1 = DirectX::XMLoadFloat3(&v1.GetValue<VertexType::Position3D>());
			const auto pos2 = DirectX::XMLoadFloat3(&v2.GetValue<VertexType::Position3D>());
	
			// Normal Vector�� ���� (�ﰢ���� ������ ���� ����)
			const auto normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(pos1, pos0), DirectX::XMVectorSubtract(pos2, pos0)));
	
			// ������ Normal Vector�� ������
			XMStoreFloat3(&v0.GetValue<VertexType::Normal>(), normal);
			XMStoreFloat3(&v1.GetValue<VertexType::Normal>(), normal);
			XMStoreFloat3(&v2.GetValue<VertexType::Normal>(), normal);
		}
	}

	VertexCore::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};