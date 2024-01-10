#include "stdafx.h"

// ========================================================
//	정점 버퍼
// ========================================================

/*
	- GPU가 정점 배열에 접근하기 위해서 정점 버퍼(Vertex Buffer)를 만들어야 한다.
	- DirectX 3D에서 버퍼를 만들기 위해서 ID3D11Buffer이라는 인터페이스를 사용한다.
	- ID3D11Buffer는 CPU와 GPU가 버퍼를 어떻게 읽고 쓰는지 여부를 알려주고 렌더링 파이프라인 중 어디에 묶이는지 알려주는 역할을 한다.
	
	정점 버퍼 생성 과정
		1. D3D11_BUFFER_DESC 구조체를 작성한다.
		2. D3D11_SUBRESOURCE_DATA 구조체를 작성한다.
		3. ID3D11Device에 있는 CreateBuffer를 호출하여 버퍼를 생성한다.

		D3D11_BUFFER_DESC
			- Usage에 설정할 수 있는 값
				1. D3D11_USAGE_DEFALUT : GPU가 자원을 읽을 때 사용 (VertexBuffer는 이 값으로 설정함)
				2. D3D11_USAGE_IMMUTABLE : GPU 또는 CPU가 자원을 읽을 때 사용
				3. D3D11_USAGE_DYNAMIC : GPU 또는 CPU가 자원을 읽고 GPU가 자원을 쓸 때 사용
				4. D3D11_USAGE_STAGING : GPU 또는 CPU에 자원을 읽고 쓸때 사용 (비효율적인 연산)
			
			- BindFlags : VertexBuffer를 제작하기 때문에 D3D11_BIND_VERTEX_BUFFER를 입력한다.
			- MiscFlags : VertexBuffer에서는 기타 플래그를 사용하지 않기 때문에 0을 입력
			- StructreByteStride : 구조적인 버퍼(Structured Buffer)에 사용하는 맴버 변수 값으로 VertexBuffer에서는 0을 설정한다.

	- VertexBuffer를 만들고나서 ID3D11DeviceContext에 있는 IASetVertexBuffer를 통해서 렌더링파이트라인에 연결한다.

	- VertexBuffer로 버퍼를 렌더링파이프라인에 연결한 후 ID3D11DeviceContext에 있는 Draw를 호출해야 정점을 그린다.
*/

namespace Sample
{
	// D3D11_BUFFER_DESC의 구조
	typedef struct D3D11_BUFFER_DESC
	{
		UINT ByteWidth;				// 생성할 정점 버퍼의 크기
		D3D11_USAGE Usage;			// 버퍼가 사용되는 방식 (CPU 또는 GPU 읽기 쓰기 여부)
		UINT BindFlags;				// 버퍼의 종류
		UINT CPUAccessFlags;		// CPU의 버퍼 접근 방식
		UINT MiscFlags;				// 기타 플레근
		UINT StructreByteStride;	// 버퍼 원소마다 데이터 크기
	} D3D11_BUFFER_DESC;

	// D3D11_SUBRESOURCE_DATA의 구조
	typedef struct D3D11_SUBRESOURCE_DATA
	{
		const void* pSystem;	// 정점 버퍼를 초기화하기 위한 시스템 메모리 배열
		UINT SysMemPitch;		// 버퍼의 한행을 나타내는 크기 (VertexBuffer에는 사용하지 않음)
		UINT SysMemSlicePitch;	// 버퍼의 총 크기 (VertexBuffer에는 사용하지 않음)
	} D3D11_SUBRESOURCE_DATA;

	// IASetVertexBuffers의 구성
	class ID3D11DeviceContext
	{
		HRESULT IASetVertexBuffers(
			UINT StartSlot,							// 정점을 붙일 입력 슬롯 색인
			UINT NumBuffers,						// 정점을 붙일 입력 솔롯의 개수
			ID3D11Buffer* const* ppVertexBuffers,	// 정점 버퍼의 주소
			const UINT* pStrides,					// 정점 버퍼 원소의 크기
			const UINT* pOffsets					// 시작 인덱스
		);
	};

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Vertex1 { };
	struct Vertex2 { };

	// VertexBuffer를 생성하는 예
	void Sample2()
	{
		Vertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), (const float*)&Colors::White },
			{ XMFLOAT3(-1.0f, +1.0f, -1.0f), (const float*)&Colors::Black },
			{ XMFLOAT3(+1.0f, +1.0f, -1.0f), (const float*)&Colors::Cyan },
			{ XMFLOAT3(+1.0f, -1.0f, -1.0f), (const float*)&Colors::Green },
			{ XMFLOAT3(-1.0f, -1.0f, +1.0f), (const float*)&Colors::Blue },
			{ XMFLOAT3(-1.0f, +1.0f, +1.0f), (const float*)&Colors::LightSteelBlue },
			{ XMFLOAT3(+1.0f, +1.0f, +1.0f), (const float*)&Colors::Magenta },
			{ XMFLOAT3(+1.0f, -1.0f, +1.0f), (const float*)&Colors::Red }
		};

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructreByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSystem = vertices;


		ID3D11Buffer* vertexBuffer;
	  //HR(device->CreateBuffer(&vertexBufferDesc, &initData, &vertexBuffer));

		// VertexBuffer를 렌더링 파이프라인에 연결하는 방법
		ID3D11Buffer* vertexbuffer1;
		ID3D11Buffer* vertexBuffer2;

		UINT stride = sizeof(Vertex1);
		UINT offset = 0;
	  //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer1, &stride, &offset);

		// Vertex1로 물체를 그림

		stride = sizeof(Vertex2);
		offset = 0;
	  //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer2, &stirde, &offset);

		// Vertex2로 물체를 그림
	};
}