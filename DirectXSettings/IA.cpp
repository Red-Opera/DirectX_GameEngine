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
}
