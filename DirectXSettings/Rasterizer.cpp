#include "stdafx.h"
#include "Rasterizer.h"

Rasterizer::Rasterizer()
{
	// 앞뒷면 모두 출력하는 Rasterizer, 앞면만 출력하는 Rasterizer
	ID3D11RasterizerState* noCullRasterizer;
	ID3D11RasterizerState* cullRasterizer;

	// 레스터화기를 만들기 위한 DESC 생성
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

	// 레스터화기 생성
	HR(dx3dApp->GetDevice()->CreateRasterizerState(&noCullrasterizerDesc, &noCullRasterizer));
	HR(dx3dApp->GetDevice()->CreateRasterizerState(&cullrasterizerDesc, &cullRasterizer));

	// 레스터화기 단계에 적용
	dx3dApp->GetDeviceContext()->RSSetState(cullRasterizer);

  //dx3dApp->GetDeviceContext()->RSSetState(noCullRasterizer); // 이 코드를 적용할 경우 다음 오브젝트를 그릴 때 모든 면이 그려짐
}