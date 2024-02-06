#pragma once

#include "Base/TriangleIndexList.h"


class Plane
{
public:
	template<class V>
	static TriangleIndexList<V> MakeTesselated(int division_x, int division_y)
	{
		assert(division_x >= 1);
		assert(division_y >= 1);

		constexpr float width = 2.0f, height = 2.0f;
		const int vertices_x = division_x + 1, vertices_y = division_y + 1;

		vector<V> vertices(vertices_x * vertices_y);
		const float side_x = width / 2.0f, side_y = height / 2;
		const float divisionSizeX = width / float(division_x), divisitionSizeY = height / float(division_y);
		const auto bottomLeft = DirectX::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

		int y = 0;
		for (int i = 0; y < vertices_y; y++)
		{
			const float y_pos = float(y) * divisitionSizeY;

			for (int x = 0; x < vertices_x; x++, i++)
			{
				const auto v = DirectX::XMVectorAdd(bottomLeft, DirectX::XMVectorSet(float(x) * divisionSizeX, y_pos, 0.0f, 0.0f));
				DirectX::XMStoreFloat3(&vertices[i].position, v);
			}
		}

		std::vector<unsigned short> indices;
		indices.reserve(pow(divisions_x * divisions_y, 2) * 6);
		{
			const auto vxy2i = [vertices_x](size_t x, size_t y) { return (unsigned short)(y * vertices_x + x); };

			for (size_t y = 0; y < division_y; y++)
			{
				for (size_t x = 0; x < division_x; x++)
				{
					const std::array<unsigned short, 4> indexArray =
					{ 
						vxy2i(x, y),
						vxy2i(x + 1, y),
						vxy2i(x, y + 1),
						vxy2i(x + 1, y + 1) 
					};

					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		return{ std::move(vertices),std::move(indices) };
	}

	template<class V>
	static TriangleIndexList<V> Make()
	{
		return MakeTesselated<V>(1, 1);
	}
};