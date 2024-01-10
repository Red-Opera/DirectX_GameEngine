#include "stdafx.h"
#include "IA.h"

D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

D3D11_INPUT_ELEMENT_DESC textureDesc[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

IA::IA()
{
	// CreateInputLaout에 들어가는 ID3D11Effect 작성
	ID3DX11Effect* fx = nullptr;
	ID3DX11EffectTechnique* tech;
	ID3D11InputLayout* inputLaout;

	tech = fx->GetTechniqueByName("Tech");

	D3DX11_PASS_DESC passDesc;
	tech->GetPassByIndex(0)->GetDesc(&passDesc);

	// InputLaout 생성
	HR(dx3dApp->GetDevice()->CreateInputLayout(vertexDesc, 4, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &inputLaout));

	// InputLayout을 렌더링 파이프라인에 연결
	dx3dApp->GetDeviceContext()->IASetInputLayout(inputLaout);


	// VertexBuffer 생성
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
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices;

	ID3D11Buffer* vertexBuffer;
	HR(dx3dApp->GetDevice()->CreateBuffer(&vertexBufferDesc, &initData, &vertexBuffer));

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dx3dApp->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dx3dApp->GetDeviceContext()->Draw(8, 0);

	// IndexBuffer 생성

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 24;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

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

  //D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;

	ID3D11Buffer* indexBuffer;
	HR(dx3dApp->GetDevice()->CreateBuffer(&indexBufferDesc, &initData, &indexBuffer));
	dx3dApp->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx3dApp->GetDeviceContext()->DrawIndexed(24, 0, 0);
}
