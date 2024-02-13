#include "stdafx.h"
#include "SamplerState.h"

#include "Core/Exception/GraphicsException.h"

SamplerState::SamplerState(DxGraphic& graphic)
{
	CREATEINFOMANAGER(graphic);

	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// Ȯ��(MAG) ���(MIN)�� �� �Ǵ� �Ӹ� ���������� ���� ������ ���
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// �Ϲ����� ũ�⺸�� Ŭ �� �ݺ����� �������� ���̵��� ����
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// Sampler State�� ������
	GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateSamplerState(&samplerDesc, &samplerState));
}

void SamplerState::PipeLineSet(DxGraphic& graphic) noexcept
{
	// SamplerState�� ������ ������ ���ο� �Է�
	GetDeviceContext(graphic)->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}
