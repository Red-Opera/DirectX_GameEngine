#include "stdafx.h"
#include "SamplerState.h"

#include "Core/Exception/GraphicsException.h"

SamplerState::SamplerState(DxGraphic& graphic)
{
	CREATEINFOMANAGER(graphic);

	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 확대(MAG) 축소(MIN)할 때 또는 밉맵 레벨에서도 선형 보간을 사용
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// 일반적인 크기보다 클 때 반복적인 패턴으로 보이도록 설정
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// Sampler State를 생성함
	GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateSamplerState(&samplerDesc, &samplerState));
}

void SamplerState::PipeLineSet(DxGraphic& graphic) noexcept
{
	// SamplerState를 렌더링 파이프 라인에 입력
	GetDeviceContext(graphic)->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}
