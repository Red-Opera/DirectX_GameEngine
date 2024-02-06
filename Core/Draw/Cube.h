#pragma once

#include "Base/TriangleIndexList.h"

class Cube
{
public:
	template<class V>
	static TriangleIndexList<V> CreateIndex()
	{
		constexpr float side = 1.0f / 2.0f;

		vector<DirectX::XMFLOAT3> vertices;
		vertices.push_back({ -side, -side, -side });
		vertices.push_back({  side, -side, -side });
		vertices.push_back({ -side,  side, -side });
		vertices.push_back({  side,  side, -side });
		vertices.push_back({ -side, -side,  side });
		vertices.push_back({  side, -side,  side });
		vertices.push_back({ -side,  side,  side });
		vertices.push_back({  side,  side,  side });

		vector<V> vertexConstnatBuffer(vertices.size());

		for (size_t i = 0; i < vertices.size(); i++)
			vertexConstnatBuffer[i].position = vertices[i];

		return { move(vertexConstnatBuffer),
			       { 0, 2, 1, 2, 3, 1,
					 1, 3, 5, 3, 7, 5,
					 2, 6, 3, 3, 6, 7,
					 4, 5, 7, 4, 7, 6,
					 0, 4, 2, 2, 4, 6,
					 0, 1, 4, 1, 5, 4 } };
	}
};
