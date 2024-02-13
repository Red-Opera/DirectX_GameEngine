#pragma once

#include "Base/TriangleIndexList.h"
#include <initializer_list>

class Cube
{
public:
	template<class V>
	static TriangleIndexList<V> CreateIndex()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(8);
		vertices[0].position = { -side, -side, -side };
		vertices[1].position = {  side, -side, -side };
		vertices[2].position = { -side,  side, -side };
		vertices[3].position = {  side,  side, -side };
		vertices[4].position = { -side, -side,  side };
		vertices[5].position = {  side, -side,  side };
		vertices[6].position = { -side,  side,  side };
		vertices[7].position = {  side,  side,  side };

		return { std::move(vertices),
			       { 0, 2, 1, 2, 3, 1,
					 1, 3, 5, 3, 7, 5,
					 2, 6, 3, 3, 6, 7,
					 4, 5, 7, 4, 7, 6,
					 0, 4, 2, 2, 4, 6,
					 0, 1, 4, 1, 5, 4 } };
	}

	template<class V>
	static TriangleIndexList<V> MakeSkinned()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(14);

		vertices[0].position = { -side,-side,-side };
		vertices[0].texCoord = { 2.0f / 3.0f,0.0f / 4.0f };
		vertices[1].position = { side,-side,-side };
		vertices[1].texCoord = { 1.0f / 3.0f,0.0f / 4.0f };
		vertices[2].position = { -side,side,-side };
		vertices[2].texCoord = { 2.0f / 3.0f,1.0f / 4.0f };
		vertices[3].position = { side,side,-side };
		vertices[3].texCoord = { 1.0f / 3.0f,1.0f / 4.0f };
		vertices[4].position = { -side,-side,side };
		vertices[4].texCoord = { 2.0f / 3.0f,3.0f / 4.0f };
		vertices[5].position = { side,-side,side };
		vertices[5].texCoord = { 1.0f / 3.0f,3.0f / 4.0f };
		vertices[6].position = { -side,side,side };
		vertices[6].texCoord = { 2.0f / 3.0f,2.0f / 4.0f };
		vertices[7].position = { side,side,side };
		vertices[7].texCoord = { 1.0f / 3.0f,2.0f / 4.0f };
		vertices[8].position = { -side,-side,-side };
		vertices[8].texCoord = { 2.0f / 3.0f,4.0f / 4.0f };
		vertices[9].position = { side,-side,-side };
		vertices[9].texCoord = { 1.0f / 3.0f,4.0f / 4.0f };
		vertices[10].position = { -side,-side,-side };
		vertices[10].texCoord = { 3.0f / 3.0f,1.0f / 4.0f };
		vertices[11].position = { -side,-side,side };
		vertices[11].texCoord = { 3.0f / 3.0f,2.0f / 4.0f };
		vertices[12].position = { side,-side,-side };
		vertices[12].texCoord = { 0.0f / 3.0f,1.0f / 4.0f };
		vertices[13].position = { side,-side,side };
		vertices[13].texCoord = { 0.0f / 3.0f,2.0f / 4.0f };

		return{
			std::move(vertices),{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
			}
		};
	}
};
