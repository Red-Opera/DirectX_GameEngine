#include "stdafx.h"
#include "ShadowSamplerState.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

namespace Graphic
{
	ShadowSamplerState::ShadowSamplerState()
	{
		for (size_t i = 0; i < 4; i++)
		{
			currentSampler = i;
			samplers[i] = CreateSampler(IsUseBilinear(), IsUseHardwarePCF());
		}

		SetBilinear(true);
		SetUseHardwarePCF(true);
	}

	void ShadowSamplerState::SetBilinear(bool bilin)
	{
		currentSampler = (currentSampler & ~0b01) | (bilin ? 0b01 : 0);
	}

	void ShadowSamplerState::SetUseHardwarePCF(bool isUseHardwarePCF)
	{
		currentSampler = (currentSampler & ~0b10) | (isUseHardwarePCF ? 0b10 : 0);
	}

	bool ShadowSamplerState::IsUseBilinear() const
	{
		return currentSampler & 0b01;
	}

	bool ShadowSamplerState::IsUseHardwarePCF() const
	{
		return currentSampler & 0b10;
	}

	void ShadowSamplerState::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetSamplers(GetCurrentSlot(), 1, samplers[currentSampler].GetAddressOf()));
	}

	UINT ShadowSamplerState::GetCurrentSlot() const
	{
		return IsUseHardwarePCF() ? 1 : 2;
	}

	size_t ShadowSamplerState::GetShadowSamplerIndex(bool bilin, bool isUseHardwarePCF)
	{
		return (bilin ? 0b01 : 0) + (isUseHardwarePCF ? 0b10 : 0);
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowSamplerState::CreateSampler(bool bilin, bool isUseHardwarePCF)
	{
		CREATEINFOMANAGER(Window::GetDxGraphic());

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

		if (isUseHardwarePCF)
		{
			samplerDesc.Filter = bilin ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		}

		else
			samplerDesc.Filter = bilin ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;

		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		// Sampler State를 생성함
		hr = GetDevice(Window::GetDxGraphic())->CreateSamplerState(&samplerDesc, &samplerState);
		GRAPHIC_THROW_INFO(hr);

		return std::move(samplerState);
	}
}