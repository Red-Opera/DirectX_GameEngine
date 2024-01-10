#include "stdafx.h"

// ========================================================
//	���� ����
// ========================================================

/*
	- GPU�� ���� �迭�� �����ϱ� ���ؼ� ���� ����(Vertex Buffer)�� ������ �Ѵ�.
	- DirectX 3D���� ���۸� ����� ���ؼ� ID3D11Buffer�̶�� �������̽��� ����Ѵ�.
	- ID3D11Buffer�� CPU�� GPU�� ���۸� ��� �а� ������ ���θ� �˷��ְ� ������ ���������� �� ��� ���̴��� �˷��ִ� ������ �Ѵ�.
	
	���� ���� ���� ����
		1. D3D11_BUFFER_DESC ����ü�� �ۼ��Ѵ�.
		2. D3D11_SUBRESOURCE_DATA ����ü�� �ۼ��Ѵ�.
		3. ID3D11Device�� �ִ� CreateBuffer�� ȣ���Ͽ� ���۸� �����Ѵ�.

		D3D11_BUFFER_DESC
			- Usage�� ������ �� �ִ� ��
				1. D3D11_USAGE_DEFALUT : GPU�� �ڿ��� ���� �� ��� (VertexBuffer�� �� ������ ������)
				2. D3D11_USAGE_IMMUTABLE : GPU �Ǵ� CPU�� �ڿ��� ���� �� ���
				3. D3D11_USAGE_DYNAMIC : GPU �Ǵ� CPU�� �ڿ��� �а� GPU�� �ڿ��� �� �� ���
				4. D3D11_USAGE_STAGING : GPU �Ǵ� CPU�� �ڿ��� �а� ���� ��� (��ȿ������ ����)
			
			- BindFlags : VertexBuffer�� �����ϱ� ������ D3D11_BIND_VERTEX_BUFFER�� �Է��Ѵ�.
			- MiscFlags : VertexBuffer������ ��Ÿ �÷��׸� ������� �ʱ� ������ 0�� �Է�
			- StructreByteStride : �������� ����(Structured Buffer)�� ����ϴ� �ɹ� ���� ������ VertexBuffer������ 0�� �����Ѵ�.

	- VertexBuffer�� ������� ID3D11DeviceContext�� �ִ� IASetVertexBuffer�� ���ؼ� ����������Ʈ���ο� �����Ѵ�.

	- VertexBuffer�� ���۸� ���������������ο� ������ �� ID3D11DeviceContext�� �ִ� Draw�� ȣ���ؾ� ������ �׸���.
*/

namespace Sample
{
	// D3D11_BUFFER_DESC�� ����
	typedef struct D3D11_BUFFER_DESC
	{
		UINT ByteWidth;				// ������ ���� ������ ũ��
		D3D11_USAGE Usage;			// ���۰� ���Ǵ� ��� (CPU �Ǵ� GPU �б� ���� ����)
		UINT BindFlags;				// ������ ����
		UINT CPUAccessFlags;		// CPU�� ���� ���� ���
		UINT MiscFlags;				// ��Ÿ �÷���
		UINT StructreByteStride;	// ���� ���Ҹ��� ������ ũ��
	} D3D11_BUFFER_DESC;

	// D3D11_SUBRESOURCE_DATA�� ����
	typedef struct D3D11_SUBRESOURCE_DATA
	{
		const void* pSystem;	// ���� ���۸� �ʱ�ȭ�ϱ� ���� �ý��� �޸� �迭
		UINT SysMemPitch;		// ������ ������ ��Ÿ���� ũ�� (VertexBuffer���� ������� ����)
		UINT SysMemSlicePitch;	// ������ �� ũ�� (VertexBuffer���� ������� ����)
	} D3D11_SUBRESOURCE_DATA;

	// IASetVertexBuffers�� ����
	class ID3D11DeviceContext
	{
		HRESULT IASetVertexBuffers(
			UINT StartSlot,							// ������ ���� �Է� ���� ����
			UINT NumBuffers,						// ������ ���� �Է� �ַ��� ����
			ID3D11Buffer* const* ppVertexBuffers,	// ���� ������ �ּ�
			const UINT* pStrides,					// ���� ���� ������ ũ��
			const UINT* pOffsets					// ���� �ε���
		);
	};

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Vertex1 { };
	struct Vertex2 { };

	// VertexBuffer�� �����ϴ� ��
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

		// VertexBuffer�� ������ ���������ο� �����ϴ� ���
		ID3D11Buffer* vertexbuffer1;
		ID3D11Buffer* vertexBuffer2;

		UINT stride = sizeof(Vertex1);
		UINT offset = 0;
	  //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer1, &stride, &offset);

		// Vertex1�� ��ü�� �׸�

		stride = sizeof(Vertex2);
		offset = 0;
	  //deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer2, &stirde, &offset);

		// Vertex2�� ��ü�� �׸�
	};
}