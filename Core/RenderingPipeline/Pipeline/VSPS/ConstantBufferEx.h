#pragma once
#include "DynamicConstantBuffer.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

namespace Graphic
{
	class ConstantBufferEx : public Render
	{
	public:
		void Update(DxGraphic& graphic, const DynamicConstantBuffer::Buffer& buffer)
		{
			assert(&buffer.GetLayout() == &GetLayout());
			CREATEINFOMANAGER(graphic);

			D3D11_MAPPED_SUBRESOURCE map;

			hr = GetDeviceContext(graphic)->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
			GRAPHIC_THROW_INFO(hr);

			memcpy(map.pData, buffer.data(), buffer.size());
			GetDeviceContext(graphic)->Unmap(constantBuffer.Get(), 0);
		}

		virtual const DynamicConstantBuffer::LayoutType& GetLayout() const noexcept = 0;

	protected:
		ConstantBufferEx(
			DxGraphic& graphic,
			const DynamicConstantBuffer::LayoutType& layoutIn,
			UINT slot,
			const DynamicConstantBuffer::Buffer* buffer) : slot(slot)
		{
			CREATEINFOMANAGER(graphic);

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

				hr = GetDevice(graphic)->CreateBuffer(&constantBufferDesc, &initData, &constantBuffer);
				GRAPHIC_THROW_INFO(hr);
			}

			else
			{
				hr = GetDevice(graphic)->CreateBuffer(&constantBufferDesc, nullptr, &constantBuffer);
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
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override 
		{ 
			CREATEINFOMANAGERNOHR(graphic);

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->PSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()));
		}
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override
		{
			CREATEINFOMANAGERNOHR(graphic);

			GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->VSSetConstantBuffers(slot, 1u, constantBuffer.GetAddressOf()));
		}
	};


	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(DxGraphic& graphic, const DynamicConstantBuffer::CompleteLayout& layout, UINT slot)
			: T(graphic, *layout.GetLayout(), slot, nullptr), 
			  buffer(DynamicConstantBuffer::Buffer(layout))
		{
		
		}
		
		CachingConstantBufferEx(DxGraphic& graphic, const DynamicConstantBuffer::Buffer& buffer, UINT slot)
			: T(graphic, buffer.GetLayout(), slot, &buffer),
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
		
		void SetRenderPipeline(DxGraphic& graphic) noexcept override
		{
			if (IsNotMatch)
			{
				T::Update(graphic, buffer);
				IsNotMatch = false;
			}
		
			T::SetRenderPipeline(graphic);
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