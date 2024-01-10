#include "stdafx.h"

// ========================================================
//	색인과 색인 버퍼
// ========================================================

/*
	- 정점 인덱스도 GPU가 접근하기 위해서 정점 버퍼를 만들어야 한다.
	- 정점 버퍼도 ID3D11DeviceContext에 있는 IASetIndexBuffer를 통해서 렌더링파이프라인에 묶어야 한다.
	
	IASetIndexBuffer
		- Format : 인덱스 버퍼에서 색인은 UINT 형 4바이트를 사용하기 때문에 DXGI_FORMAT_R32_UINT로 작성(DXGI_FORMAT_R16_UINT도 가능)한다.

	- 색인 버퍼를 이용하여 기하구조를 그릴 경우 ID3D11DeviceContext에 있는 DrawIndexed를 이용한다.

	DrawIndexed에서 두번째. 3번째 매개변수가 존재하는 이유
		- IASetIndexBuffer와 같은 API를 호출은 비용이 비싸기 때문에 여러개의 물체를 하나로 합치기도 한다.
		- IASetIndexBuffer호출을 줄이기 위해서는 하나의 인덱스 버퍼와 버텍스 버퍼로 합친다.
		- 이럴경우 DrawIndexed에 있는 두번째 인자값을 통해서 IndexBuffer의 해당 물체 인덱스 시작점을 알려주고 세번째 인자값이 VertexBuffer의 인덱스 시작점을 알려준다.
*/

namespace Sample
{
	class ID3D11DeviceContext
	{
		// IASetIndexBuffer의 구조
		void IASetIndexBuffer(
			ID3D11Buffer* pIndexBuffer,	// IndexBuffer
			DXGI_FORMAT Format,			// IndexBuffer의 원소당 메모리 크기
			UINT Offset					// 시작하는 인덱스 위치
		);

		// DrawIndexed의 구조
		void DrawIndexed(
			UINT IndexCount,			// 그릴 인덱스 버퍼 (모두 그릴 필요 없음)
			UINT StartIndexLocation,	// 그릴 시작 인덱스
			INT BaseVertexLocation		// 시작하는 정점 인덱스 위치
		);
	};

	UINT indices[24] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		0, 7, 8,
		0, 8, 1
	};

	// 인덱스 버퍼 만드는 과정
	void Sample3()
	{
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.ByteWidth = sizeof(UINT) * 24;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = indices;

		ID3D11Buffer* indexBuffer;
	  //HR(device->CreateBuffer(&indexBufferDesc, &initData, &indexBuffer));
	  //deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	  //deviceContext->DrawIndexed(24, 0, 0);
	}
}