#pragma once
#include "../Bindable.h"

#include "../../Exception/GraphicsException.h"

template<typename BufferType>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(DxGraphic& graphic, const BufferType& bufferList, UINT slot = 0u) : slot(slot)
	{
		CREATEINFOMANAGER(graphic);

		// 상수 버퍼 생성 
		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.ByteWidth = sizeof(bufferList);
		constantBufferDesc.StructureByteStride = 0;
		constantBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = &bufferList;

		GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer));
	}

	ConstantBuffer(DxGraphic& graphic, UINT slot = 0u) : slot(slot)
	{
		CREATEINFOMANAGER(graphic);

		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.ByteWidth = sizeof(BufferType);
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer));
	}

	void Update(DxGraphic& graphic, const BufferType& consts)
	{
		CREATEINFOMANAGER(graphic);

		D3D11_MAPPED_SUBRESOURCE map;
		GRAPHIC_THROW_INFO(GetDeviceContext(graphic)->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map));

		memcpy(map.pData, &consts, sizeof(consts));
		GetDeviceContext(graphic)->Unmap(constantBuffer.Get(), 0);
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	UINT slot;
};

// Vertex Constant Buffer 생성 클래스
template<typename BufferType>
class VertexConstantBuffer : public ConstantBuffer<BufferType>
{
	// 기존에 있는 상수 버퍼 변수와 Device Context를 가져오는 코드를 그대로 사용
	using ConstantBuffer<BufferType>::constantBuffer;
	using ConstantBuffer<BufferType>::slot;
	using Bindable::GetDeviceContext;

public:
	using ConstantBuffer<BufferType>::ConstantBuffer;
	
	void PipeLineSet(DxGraphic& graphic) noexcept override
	{
		GetDeviceContext(graphic)->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
	}
};

// Pixel Constant Buffer 생성 클래스
template<typename BufferType>
class PixelConstantBuffer : public ConstantBuffer<BufferType>
{
	using ConstantBuffer<BufferType>::constantBuffer;
	using ConstantBuffer<BufferType>::slot;
	using Bindable::GetDeviceContext;

public:
	using ConstantBuffer<BufferType>::ConstantBuffer;
	
	void PipeLineSet(DxGraphic& graphic)noexcept override
	{
		GetDeviceContext(graphic)->PSSetConstantBuffers(slot, 1, constantBuffer.GetAddressOf());
	}
};