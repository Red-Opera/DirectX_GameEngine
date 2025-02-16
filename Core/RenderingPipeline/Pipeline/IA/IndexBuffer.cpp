#include "stdafx.h"
#include "IndexBuffer.h"

#include "Core/RenderingPipeline/RenderManager.h"

#include "Core/Exception/GraphicsException.h"
using namespace std;

namespace Graphic
{
	IndexBuffer::IndexBuffer(DxGraphic& graphic, const vector<unsigned short>& indices) : IndexBuffer(graphic, "?", indices)
	{

	}

	IndexBuffer::IndexBuffer(DxGraphic& graphic, std::string path, const std::vector<USHORT>& indices) : indexCount((UINT)indices.size()), path(path)
	{
		CREATEINFOMANAGER(graphic);

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.ByteWidth = UINT(indexCount * sizeof(unsigned short));
		indexBufferDesc.StructureByteStride = sizeof(unsigned short);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = indices.data();

		hr = GetDevice(graphic)->CreateBuffer(&indexBufferDesc, &initData, &indexBuffer);
		GRAPHIC_THROW_INFO(hr);
	}

	void IndexBuffer::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(graphic);

		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0));
	}

	UINT IndexBuffer::GetIndexCount() const noexcept
	{
		return indexCount;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::GetRender(DxGraphic& graphic, const std::string& path, const std::vector<USHORT>& indices)
	{
		assert(path != "?");

		return RenderManager::GetRender<IndexBuffer>(graphic, path, indices);
	}

	std::string IndexBuffer::GetID() const noexcept
	{
		return CreateID_(path);
	}

	std::string IndexBuffer::CreateID_(const std::string& path)
	{
		using namespace std::string_literals;

		return typeid(IndexBuffer).name() + "#"s + path;
	}
}