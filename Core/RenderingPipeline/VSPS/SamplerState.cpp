#include "stdafx.h"
#include "SamplerState.h"

#include "Core/Exception/GraphicsException.h"

#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
	SamplerState::SamplerState(DxGraphic& graphic)
	{
		CREATEINFOMANAGER(graphic);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;			// 확대(MAG) 축소(MIN)할 때 사용할 필터링 방법
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// 일반적인 크기보다 클 때 반복적인 패턴으로 보이도록 설정
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;	// 필터링에 사용할 최대 샘플링 비율 (1 ~ 16)
		//samplerDesc.MipLODBias = 0.0f;							// 밉맵 레벨을 결정할 때 사용할 바이어스 값
		//samplerDesc.MinLOD = 0.0f;								// 텍스처의 최소 LOD 레벨
		//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;					// 텍스처의 최대 LOD 레벨

		// Sampler State를 생성함
		hr = GetDevice(graphic)->CreateSamplerState(&samplerDesc, &samplerState);
		GRAPHIC_THROW_INFO(hr);
	}

	void SamplerState::PipeLineSet(DxGraphic& graphic) noexcept
	{
		// SamplerState를 렌더링 파이프 라인에 입력
		GetDeviceContext(graphic)->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	}

	std::shared_ptr<SamplerState> SamplerState::GetRender(DxGraphic& graphic)
	{
		return RenderManager::GetRender<SamplerState>(graphic);
	}

	std::string SamplerState::CreateID()
	{
		return typeid(SamplerState).name();
	}

	std::string SamplerState::GetID() const noexcept
	{
		return CreateID();
	}
}