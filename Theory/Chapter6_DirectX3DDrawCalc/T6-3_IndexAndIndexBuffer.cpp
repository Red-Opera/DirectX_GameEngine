#include "stdafx.h"

// ========================================================
//	���ΰ� ���� ����
// ========================================================

/*
	- ���� �ε����� GPU�� �����ϱ� ���ؼ� ���� ���۸� ������ �Ѵ�.
	- ���� ���۵� ID3D11DeviceContext�� �ִ� IASetIndexBuffer�� ���ؼ� ���������������ο� ����� �Ѵ�.
	
	IASetIndexBuffer
		- Format : �ε��� ���ۿ��� ������ UINT �� 4����Ʈ�� ����ϱ� ������ DXGI_FORMAT_R32_UINT�� �ۼ�(DXGI_FORMAT_R16_UINT�� ����)�Ѵ�.

	- ���� ���۸� �̿��Ͽ� ���ϱ����� �׸� ��� ID3D11DeviceContext�� �ִ� DrawIndexed�� �̿��Ѵ�.

	DrawIndexed���� �ι�°. 3��° �Ű������� �����ϴ� ����
		- IASetIndexBuffer�� ���� API�� ȣ���� ����� ��α� ������ �������� ��ü�� �ϳ��� ��ġ�⵵ �Ѵ�.
		- IASetIndexBufferȣ���� ���̱� ���ؼ��� �ϳ��� �ε��� ���ۿ� ���ؽ� ���۷� ��ģ��.
		- �̷���� DrawIndexed�� �ִ� �ι�° ���ڰ��� ���ؼ� IndexBuffer�� �ش� ��ü �ε��� �������� �˷��ְ� ����° ���ڰ��� VertexBuffer�� �ε��� �������� �˷��ش�.
*/

namespace Sample
{
	class ID3D11DeviceContext
	{
		// IASetIndexBuffer�� ����
		void IASetIndexBuffer(
			ID3D11Buffer* pIndexBuffer,	// IndexBuffer
			DXGI_FORMAT Format,			// IndexBuffer�� ���Ҵ� �޸� ũ��
			UINT Offset					// �����ϴ� �ε��� ��ġ
		);

		// DrawIndexed�� ����
		void DrawIndexed(
			UINT IndexCount,			// �׸� �ε��� ���� (��� �׸� �ʿ� ����)
			UINT StartIndexLocation,	// �׸� ���� �ε���
			INT BaseVertexLocation		// �����ϴ� ���� �ε��� ��ġ
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

	// �ε��� ���� ����� ����
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