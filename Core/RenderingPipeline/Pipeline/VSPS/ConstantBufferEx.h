#pragma once
#include "DynamicConstantBuffer.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"
#include "Core/Window.h"

namespace Graphic
{
	class ConstantBufferEx : public Render
	{
	public:
		void Update(const DynamicConstantBuffer::Buffer& buffer)
		{
			assert(&buffer.GetLayout() == &GetLayout());
			CREATEINFOMANAGER(Window::GetDxGraphic());

			D3D11_MAPPED_SUBRESOURCE map;

			hr = GetDeviceContext(Window::GetDxGraphic())->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			GRAPHIC_THROW_INFO(hr);

			memcpy(map.pData, buffer.data(), buffer.size());
			GetDeviceContext(Window::GetDxGraphic())->Unmap(constantBuffer.Get(), 0);
		}

		virtual const DynamicConstantBuffer::LayoutType& GetLayout() const noexcept = 0;

	protected:
		ConstantBufferEx(
			const DynamicConstantBuffer::LayoutType& layoutIn,
			UINT slot,
			const DynamicConstantBuffer::Buffer* buffer) : slot(slot)
		{
			CREATEINFOMANAGER(Window::GetDxGraphic());

			D3D11_BUFFER_DESC constantBufferDesc;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDesc.ByteWidth = (UINT)layoutIn.GetLayoutByteSize();
			constantBufferDesc.StructureByteStride = 0;
			constantBufferDesc.MiscFlags = 0;

			if (buffer != nullptr)
			{
				D3D11_SUBRESOURCE_DATA initData = {};
				initData.pSysMem = buffer->data();

				hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer);
				GRAPHIC_THROW_INFO(hr);
			}

			else
			{
				hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
				GRAPHIC_THROW_INFO(hr);
			}
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
		UINT slot;
	};

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override 
		{ 
			CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()));
		}
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline() NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(Window::GetDxGraphic())->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()));
		}
	};


	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(const DynamicConstantBuffer::CompleteLayout& layout, UINT slot)
			: T(*layout.GetLayout(), slot, nullptr), 
			  buffer(DynamicConstantBuffer::Buffer(layout))
		{
		
		}
		
		CachingConstantBufferEx(const DynamicConstantBuffer::Buffer& buffer, UINT slot)
			: T(buffer.GetLayout(), slot, &buffer),
			  buffer(buffer)
		{
		
		}
		
		const DynamicConstantBuffer::LayoutType& GetLayout() const noexcept override
		{
			return buffer.GetLayout();
		}
		
		const DynamicConstantBuffer::Buffer& GetBuffer() const noexcept
		{
			return buffer;
		}
		
		void SetBuffer(const DynamicConstantBuffer::Buffer& bufferIn)
		{
			buffer.set(bufferIn);
			IsNotMatch = true;
		}
		
		void SetRenderPipeline() noexcept override
		{
			if (IsNotMatch)
			{
				T::Update(buffer);
				IsNotMatch = false;
			}
		
			T::SetRenderPipeline();
		}
		
		void Accept(TechniqueBase& tech) override
		{
			if (tech.VisitBuffer(buffer))
				IsNotMatch = true;
		}

	private:
		bool IsNotMatch = false;
		DynamicConstantBuffer::Buffer buffer;
	};

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;
}