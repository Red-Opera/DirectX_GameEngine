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

	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};

