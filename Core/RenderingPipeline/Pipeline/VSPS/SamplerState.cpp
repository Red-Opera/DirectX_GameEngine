#include "stdafx.h"
#include "SamplerState.h"

#include "Core/Exception/GraphicsException.h"

#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
	SamplerState::SamplerState(DxGraphic& graphic, TextureFilter textureFilter, bool useReflect, UINT slot)
		: textureFilter(textureFilter), useReflect(useReflect), slot(slot)
	{
		CREATEINFOMANAGER(graphic);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = [textureFilter]()			// Ȯ��(MAG) ���(MIN)�� �� ����� ���͸� ���
		{
			switch (textureFilter)
			{
			case TextureFilter::Anisotropic:
				return D3D11_FILTER_ANISOTROPIC;

			case TextureFilter::Point:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;

			default:
			case TextureFilter::Bilinear:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}();

		samplerDesc.AddressU = useReflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;		// �Ϲ����� ũ�⺸�� Ŭ �� �ݺ����� �������� ���̵��� ����
		samplerDesc.AddressV = useReflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;	// ���͸��� ����� �ִ� ���ø� ���� (1 ~ 16)
		//samplerDesc.MipLODBias = 0.0f;							// �Ӹ� ������ ������ �� ����� ���̾ ��
		//samplerDesc.MinLOD = 0.0f;								// �ؽ�ó�� �ּ� LOD ����
		//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;					// �ؽ�ó�� �ִ� LOD ����

		// Sampler State�� ������
		hr = GetDevice(graphic)->CreateSamplerState(&samplerDesc, &samplerState);
		GRAPHIC_THROW_INFO(hr);
	}

	void SamplerState::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);

		// SamplerState�� ������ ������ ���ο� �Է�
		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetSamplers(slot, 1, samplerState.GetAddressOf()));
	}

	std::shared_ptr<SamplerState> SamplerState::GetRender(DxGraphic& graphic, TextureFilter textureFilter, bool useReflect, UINT slot)
	{
		return RenderManager::GetRender<SamplerState>(graphic, textureFilter, useReflect, slot);
	}

	std::string SamplerState::CreateID(TextureFilter textureFilter, bool useReflect, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(SamplerState).name() + "#"s + std::to_string((int)textureFilter) + (useReflect ? "R"s : "w"s) + "@"s + std::to_string(slot);
	}

	std::string SamplerState::GetID() const noexcept
	{
		return CreateID(textureFilter, useReflect, slot);
	}
}