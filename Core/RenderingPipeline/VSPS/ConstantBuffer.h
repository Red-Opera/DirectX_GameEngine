#pragma once
#include "../Render.h"
#include "../RenderManager.h"

#include "../../Exception/GraphicsException.h"

namespace Graphic
{
	template<typename BufferType>
	class ConstantBuffer : public Render
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

			hr = GetDevice(graphic)->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer);
			GRAPHIC_THROW_INFO(hr);
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

			hr = GetDevice(graphic)->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
			GRAPHIC_THROW_INFO(hr);
		}

		void Update(DxGraphic& graphic, const BufferType& consts)
		{
			CREATEINFOMANAGER(graphic);

			D3D11_MAPPED_SUBRESOURCE map;

			hr = GetDeviceContext(graphic)->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			GRAPHIC_THROW_INFO(hr);

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
		using Render::GetDeviceContext;

	public:
		using ConstantBuffer<BufferType>::ConstantBuffer;

		void PipeLineSet(DxGraphic& graphic) noexcept override
		{
			GetDeviceContext(graphic)->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf());
		}

		static std::shared_ptr<VertexConstantBuffer> GetRender(DxGraphic& graphic, const BufferType& constBuffers, UINT slot = 0)
		{
			return RenderManager::GetRender<VertexConstantBuffer>(graphic, constBuffers, slot);
		}

		static std::shared_ptr<VertexConstantBuffer> GetRender(DxGraphic& graphic, UINT slot = 0)
		{
			return RenderManager::GetRender<VertexConstantBuffer>(graphic, slot);
		}

		static std::string CreateID(const BufferType&, UINT slot) { return CreateID(slot); }

		static std::string CreateID(UINT slot = 0) 
		{ 
			using namespace std::string_literals;

			return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}

		std::string GetID() const noexcept override { return CreateID(slot); }
	};

	// Pixel Constant Buffer 생성 클래스
	template<typename BufferType>
	class PixelConstantBuffer : public ConstantBuffer<BufferType>
	{
		using ConstantBuffer<BufferType>::constantBuffer;
		using ConstantBuffer<BufferType>::slot;
		using Render::GetDeviceContext;

	public:
		using ConstantBuffer<BufferType>::ConstantBuffer;

		void PipeLineSet(DxGraphic& graphic)noexcept override
		{
			GetDeviceContext(graphic)->PSSetConstantBuffers(slot, 1, constantBuffer.GetAddressOf());
		}

		static std::shared_ptr<PixelConstantBuffer> GetRender(DxGraphic& graphic, const BufferType& constBuffers, UINT slot = 0)
		{
			return RenderManager::GetRender<PixelConstantBuffer>(graphic, constBuffers, slot);
		}

		static std::shared_ptr<PixelConstantBuffer> GetRender(DxGraphic& graphic, UINT slot = 0)
		{
			return RenderManager::GetRender<PixelConstantBuffer>(graphic, slot);
		}

		static std::string CreateID(const BufferType&, UINT slot) { return CreateID(slot); }

		static std::string CreateID(UINT slot = 0)
		{
			using namespace std::string_literals;

			return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
		}

		std::string GetID() const noexcept override { return CreateID(slot); }
	};
}