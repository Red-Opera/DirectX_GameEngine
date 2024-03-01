#pragma once

#include <vector>
#include <DirectXMath.h>

template<class T>
class TriangleIndexList
{
public:
	TriangleIndexList() = default;
	TriangleIndexList(std::vector<T> vertices, std::vector<unsigned short> indices) : vertices(vertices), indices(indices)
	{
		assert("�ﰢ���� ���� ������ �����մϴ�." && vertices.size() > 2);
		assert("�ﰢ���� �ε����� ���� �ʽ��ϴ�." && (indices.size() % 3 == 0));
	}

	void Transform(DirectX::XMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.position, DirectX::XMVector3Transform(pos, matrix));
		}
	}

	// Normal Vector�� ���ϰ� �����ϴ� �޼ҵ�
	void SetNormalVector() noexcept(!_DEBUG)
	{
		assert(indices.size() % 3 == 0 && indices.size() > 0);

		// ������Ʈ�� ��� �鿡 ���ؼ� Normal Vector�� ����
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			// �ﰢ���� �� ������ ������
			auto& v0 = vertices[indices[i]];
			auto& v1 = vertices[indices[i + 1]];
			auto& v2 = vertices[indices[i + 2]];

			// ������ ��ġ�� ������
			const auto pos0 = DirectX::XMLoadFloat3(&v0.position);
			const auto pos1 = DirectX::XMLoadFloat3(&v1.position);
			const auto pos2 = DirectX::XMLoadFloat3(&v2.position);

			// Normal Vector�� ���� (�ﰢ���� ������ ���� ����)
			const auto normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(pos1, pos0), DirectX::XMVectorSubtract(pos2, pos0)));

			// ������ Normal Vector�� ������
			XMStoreFloat3(&v0.normal, normal);
			XMStoreFloat3(&v1.normal, normal);
			XMStoreFloat3(&v2.normal, normal);
		}
	}

	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};

