#include "stdafx.h"
#include "Rasterizer.h"

Rasterizer::Rasterizer()
{
	// �յ޸� ��� ����ϴ� Rasterizer, �ո鸸 ����ϴ� Rasterizer
	ID3D11RasterizerState* noCullRasterizer;
	ID3D11RasterizerState* cullRasterizer;

	// ������ȭ�⸦ ����� ���� DESC ����
	D3D11_RASTERIZER_DESC noCullrasterizerDesc;
	ZeroMemory(&noCullrasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	noCullrasterizerDesc.FillMode = D3D11_FILL_SOLID;
	noCullrasterizerDesc.CullMode = D3D11_CULL_NONE;
	noCullrasterizerDesc.FrontCounterClockwise = false;
	noCullrasterizerDesc.DepthClipEnable = true;

	D3D11_RASTERIZER_DESC cullrasterizerDesc;
	ZeroMemory(&cullrasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	cullrasterizerDesc.FillMode = D3D11_FILL_SOLID;
	cullrasterizerDesc.CullMode = D3D11_CULL_BACK;
	cullrasterizerDesc.FrontCounterClockwise = false;
	cullrasterizerDesc.DepthClipEnable = true;

	// ������ȭ�� ����
	HR(dx3dApp->GetDevice()->CreateRasterizerState(&noCullrasterizerDesc, &noCullRasterizer));
	HR(dx3dApp->GetDevice()->CreateRasterizerState(&cullrasterizerDesc, &cullRasterizer));

	// ������ȭ�� �ܰ迡 ����
	dx3dApp->GetDeviceContext()->RSSetState(cullRasterizer);

  //dx3dApp->GetDeviceContext()->RSSetState(noCullRasterizer); // �� �ڵ带 ������ ��� ���� ������Ʈ�� �׸� �� ��� ���� �׷���
}