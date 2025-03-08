#pragma once

#include "Core/Exception/GraphicsException.h"

#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"

namespace Graphic
{
	template<typename BufferType>
	class ConstantBuffer : public Render
	{
	public:
		ConstantBuffer(const BufferType& bufferList, UINT slot = 0u) : slot(slot)
		{
			CREATEINFOMANAGER(Window::GetDxGraphic());

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

			hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer);
			GRAPHIC_THROW_INFO(hr);
		}

		ConstantBuffer(UINT slot = 0u) : slot(slot)
		{
			CREATEINFOMANAGER(Window::GetDxGraphic());

			D3D11_BUFFER_DESC constantBufferDesc;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDesc.ByteWidth = sizeof(BufferType);
			constantBufferDesc.MiscFlags = 0;
			constantBufferDesc.StructureByteStride = 0;

			hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
			GRAPHIC_THROW_INFO(hr);
		}

		void Update(const BufferType& consts)
		{
			CREATEINFOMANAGER(Window::GetDxGraphic());

			D3D11_MAPPED_SUBRESOURCE map;

			hr = GetDeviceContext(Window::GetDxGraphic())->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			GRAPHIC_THROW_INFO(hr);

			memcpy(map.pData, &consts, sizeof(consts));
			GetDeviceContext(Window::GetDxGraphic())->Unmap(constantBuffer.Get(), 0);
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
		using ConstantBuffer<BufferType>::GetInfoManager;
		using Render::GetDeviceContext;

	public:
		using ConstantBuffer<BufferType>::ConstantBuffer;

		void SetRenderPipeline() NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()));
		}

		static std::shared_ptr<VertexConstantBuffer> GetRender(const BufferType& constBuffers, UINT slot = 0)
		{
			return RenderManager::GetRender<VertexConstantBuffer>(constBuffers, slot);
		}

		static std::shared_ptr<VertexConstantBuffer> GetRender(UINT slot = 0)
		{
			return RenderManager::GetRender<VertexConstantBuffer>(slot);
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
		using ConstantBuffer<BufferType>::GetInfoManager;
		using Render::GetDeviceContext;

	public:
		using ConstantBuffer<BufferType>::ConstantBuffer;

		void SetRenderPipeline() NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()));
		}

		static std::shared_ptr<PixelConstantBuffer> GetRender(const BufferType& constBuffers, UINT slot = 0)
		{
			return RenderManager::GetRender<PixelConstantBuffer>(constBuffers, slot);
		}

		static std::shared_ptr<PixelConstantBuffer> GetRender(UINT slot = 0)
		{
			return RenderManager::GetRender<PixelConstantBuffer>(slot);
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