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

	template<class V>
	static TriangleIndexList<V> MakeIndependentVertex()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(24);
		vertices[0].position = { -side,-side,-side };// 0 near side
		vertices[1].position = { side,-side,-side };	// 1
		vertices[2].position = { -side,side,-side };	// 2
		vertices[3].position = { side,side,-side };	// 3
		vertices[4].position = { -side,-side,side };	// 4 far side
		vertices[5].position = { side,-side,side };	// 5
		vertices[6].position = { -side,side,side };	// 6
		vertices[7].position = { side,side,side };	// 7
		vertices[8].position = { -side,-side,-side };// 8 left side
		vertices[9].position = { -side,side,-side };	// 9
		vertices[10].position = { -side,-side,side };// 10
		vertices[11].position = { -side,side,side };	// 11
		vertices[12].position = { side,-side,-side };// 12 right side
		vertices[13].position = { side,side,-side };	// 13
		vertices[14].position = { side,-side,side };	// 14
		vertices[15].position = { side,side,side };	// 15
		vertices[16].position = { -side,-side,-side };// 16 bottom side
		vertices[17].position = { side,-side,-side };// 17
		vertices[18].position = { -side,-side,side };// 18
		vertices[19].position = { side,-side,side };	// 19
		vertices[20].position = { -side,side,-side };// 20 top side
		vertices[21].position = { side,side,-side };	// 21
		vertices[22].position = { -side,side,side };	// 22
		vertices[23].position = { side,side,side };	// 23

		return{
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

	template<class V>
	static TriangleIndexList<V> MakeIndependentTextured()
	{
		auto itl = MakeIndependentVertex<V>();

		itl.vertices[0].textureCoord = { 0.0f,0.0f };
		itl.vertices[1].textureCoord = { 1.0f,0.0f };
		itl.vertices[2].textureCoord = { 0.0f,1.0f };
		itl.vertices[3].textureCoord = { 1.0f,1.0f };
		itl.vertices[4].textureCoord = { 0.0f,0.0f };
		itl.vertices[5].textureCoord = { 1.0f,0.0f };
		itl.vertices[6].textureCoord = { 0.0f,1.0f };
		itl.vertices[7].textureCoord = { 1.0f,1.0f };
		itl.vertices[8].textureCoord = { 0.0f,0.0f };
		itl.vertices[9].textureCoord = { 1.0f,0.0f };
		itl.vertices[10].textureCoord = { 0.0f,1.0f };
		itl.vertices[11].textureCoord = { 1.0f,1.0f };
		itl.vertices[12].textureCoord = { 0.0f,0.0f };
		itl.vertices[13].textureCoord = { 1.0f,0.0f };
		itl.vertices[14].textureCoord = { 0.0f,1.0f };
		itl.vertices[15].textureCoord = { 1.0f,1.0f };
		itl.vertices[16].textureCoord = { 0.0f,0.0f };
		itl.vertices[17].textureCoord = { 1.0f,0.0f };
		itl.vertices[18].textureCoord = { 0.0f,1.0f };
		itl.vertices[19].textureCoord = { 1.0f,1.0f };
		itl.vertices[20].textureCoord = { 0.0f,0.0f };
		itl.vertices[21].textureCoord = { 1.0f,0.0f };
		itl.vertices[22].textureCoord = { 0.0f,1.0f };
		itl.vertices[23].textureCoord = { 1.0f,1.0f };

		return itl;
	}
};
