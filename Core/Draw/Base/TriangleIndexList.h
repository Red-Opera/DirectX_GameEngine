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
		assert("삼각형의 정점 개수가 부족합니다." && vertices.size() > 2);
		assert("삼각형의 인덱스가 맞지 않습니다." && (indices.size() % 3 == 0));
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

