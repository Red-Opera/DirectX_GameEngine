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

	// Normal Vector를 구하고 저장하는 메소드
	void SetNormalVector() noexcept(!_DEBUG)
	{
		assert(indices.size() % 3 == 0 && indices.size() > 0);

		// 오브젝트의 모든 면에 대해서 Normal Vector를 구함
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			// 삼각형의 각 정점을 가져옴
			auto& v0 = vertices[indices[i]];
			auto& v1 = vertices[indices[i + 1]];
			auto& v2 = vertices[indices[i + 2]];

			// 정점의 위치를 가져옴
			const auto pos0 = DirectX::XMLoadFloat3(&v0.position);
			const auto pos1 = DirectX::XMLoadFloat3(&v1.position);
			const auto pos2 = DirectX::XMLoadFloat3(&v2.position);

			// Normal Vector를 구함 (삼각형의 외적을 통해 구함)
			const auto normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(pos1, pos0), DirectX::XMVectorSubtract(pos2, pos0)));

			// 정점의 Normal Vector를 돌려줌
			XMStoreFloat3(&v0.normal, normal);
			XMStoreFloat3(&v1.normal, normal);
			XMStoreFloat3(&v2.normal, normal);
		}
	}

	std::vector<T> vertices;
	std::vector<unsigned short> indices;
};

