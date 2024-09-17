#include "stdafx.h"
#include "VertexBuffer.h"

#include "../RenderManager.h"

namespace Graphic
{
	VertexBuffer::VertexBuffer(DxGraphic& graphic, const VertexCore::VertexBuffer& vertexBuffer) : VertexBuffer(graphic, "?", vertexBuffer)
    {
        
    }

    VertexBuffer::VertexBuffer(DxGraphic& graphic, const std::string& path, const VertexCore::VertexBuffer& vertexBuffer) 
        : stride((UINT)vertexBuffer.GetVertexLayout().size()), path(path)
    {
        CREATEINFOMANAGER(graphic);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.ByteWidth = UINT(vertexBuffer.size());
        vertexBufferDesc.StructureByteStride = stride;

        D3D11_SUBRESOURCE_DATA initData = { };
        initData.pSysMem = vertexBuffer.data();

        hr = GetDevice(graphic)->CreateBuffer(&vertexBufferDesc, &initData, &(this->vertexBuffer));
        GRAPHIC_THROW_INFO(hr);
    }

	void VertexBuffer::PipeLineSet(DxGraphic& graphic) noexcept
	{
		const UINT offset = 0;

		GetDeviceContext(graphic)->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	}

    std::shared_ptr<VertexBuffer> VertexBuffer::GetRender(DxGraphic& graphic, const std::string& path, const VertexCore::VertexBuffer& vertexBuffer)
    {
        assert(path != "?");

        return RenderManager::GetRender<VertexBuffer>(graphic, path, vertexBuffer);
    }

    std::string VertexBuffer::GetID() const noexcept
    {
        return CreateID(path);
    }

    std::string VertexBuffer::CreateID_(const std::string& path)
    {
        using namespace std::string_literals;

        return typeid(VertexBuffer).name() + "#"s + path;
    }
}