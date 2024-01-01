#include "stdafx.h"

// ========================================================
//	입력 조립기 단계 (Input Assembler, IA)
// ========================================================

/*
	- 메모리부터 기하학적 도형을 조립하는 단계
*/

// ========================================================
//	정점 (Vertex)
// ========================================================

/*
	- 삼각형에서 두변이 만나는 점
	- 선분일 경우 양 끝이 정점이고, 하나의 점일 경우 그 점 자체가 정점
	- 위치 정보 이외에 다른 정보를 담고 있더 더 복잡한 렌더링 효과를 볼 수 있음
*/

// ========================================================
//	기본도형 위상구조
// ========================================================

/*
	- 정점들은 DirectX 3D 자료구조로 있는 정점 버퍼를 이용하여 렌더링 파이프라인에 묶인다.
	- 정점 자체로 어떤 도형을 형성하는지 모르기 때문에 기본도형 위상구조(Primitive Topology)를 통해서 DirectX 3D에게 알려준다.
	- 기본도형 위상구조를 지정하는 메소드는 ID3D11DeviceContext::IASetPrimitiveTopology를 이용한다.
	- ID3D11DeviceContext::IASetPrimitiveTopology 메소드를 호출하면 다음 이 함수를 호출할 때까지 해당 위상구조가 유지된다.
	- 거의 삼각형 목록을 사용한다.

	D3D11_PRIMITIVE_TOPOLOGY enum 값
		1. D3D11_PRIMITIVE_TOPOLOGY_POINTLIST : 모든 정점은 개별적이 점으로 그려지는 값
		2. D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP : 0번부터 n번까지 입력한 모든 정점들을 순서대로 잇는 값
		3. D3D11_PRIMITIVE_TOPOLOGY_LINELIST : 0번은 1번, 2번은 3번.. 식으로 연결되며 총 n/2개의 선분이 만드는 값
		4. D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP : i, i - 1, i - 2번째 정점을 서로 연결하는 방식으로 인접한 두 삼각형이 정점을 공유하는 삼삭형을 만드는 값
		5. D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : 0, 1, 2번 연결, 3, 4, 5번 연결... 식으로 총 n / 3개의 삼각형이 만드는 값
		6. D3D11_PRIMITIVE_TOPOLOGY_XXXXXX_ADJ
			- 삼각형에서 각 변을 공유하는 삼각형을 3개를 만들 수 있으며 이러한 삼각형 각각을 인접 삼각형(Adjacent Triangles)이라고 한다.
			- 인접 삼각형은 기하 셰이더에서 인접 삼각형들에 접근해야하는 특정한 기하 셰이딩 알고리즘을 구현할 때 사용
			- 기하 셰이더 입력으로만 사용될 뿐 실제로 그려지지 않음

		7. D3D11_PRIMITIVE_TOPOLOGY_N_CONTROL_POINT_PATCHLIST : 정점 자료를 N개의 제어점(Control Point)로 이루어진 패치 목록으로 테셀레이션 단계에 사용
*/

namespace Sample
{
	// 기본도형 위상구조를 설정하는 메소드
	class ID3D11DeviceContext
	{
		void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
	};

	// IASetPrimitiveTopology에 설정할 수 있는 값
	typedef enum D3D11_PRIMITIVE_TOPOLOGY
	{
		D3D11_PRIMITIVE_TOPOLOGY_UNDEFIND = 0,
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST = 1,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST = 2,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,



		D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATTHLIST = 64
	} D3D11_PRIMITIVE_TOPOLOGY;
}

// ========================================================
//	색인
// ========================================================

/*
	- 삼각형 만들 때 색인 순서대로 정점을 연결하여 제작한다.
	
	기존 방식
		1. 연결할 정점 순서를 저장하는 정점 배열을 제작한다.
		2. 연결하는 순서대로 배열에 저장한다.

		기존 방식의 단점
			- 연결할 순서를 지정하는데 메모리 사용량이 많다.
			- 같은 정점 데이터를 처리하기 때문에 비효율적이다.

	개선 방식
		1. 정점들을 배열에 입력한다.
		2. 정점들을 연결하는 순서는 인덱스 번호로 설정한다.
*/

namespace Sample
{
	class Vertex 
	{ 
		// 정점 정보들
	};

	// 정점들을 연결하여 삼각형을 만드는 기존 방법
	Vertex v0, v1, v2, v3, v4, v5, v6, v7, v8;
	Vertex octagon[24] =
	{
		v0, v1, v2,
		v0, v2, v3,
		v0, v3, v4,
		v0, v4, v5,
		v0, v6, v7,
		v0, v7, v8,
		v0, v8, v1
	};

	// 개선한 방법
	Vertex v[9] = { v0, v1, v2, v3, v4, v5, v6, v7, v8 };
	UINT indexList[24] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5 ,6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 1
	};
}