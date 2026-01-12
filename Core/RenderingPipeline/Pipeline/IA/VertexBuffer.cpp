#include "stdafx.h"
#include "VertexBuffer.h"

#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/Window.h"

namespace Graphic
{
	VertexBuffer::VertexBuffer(const VertexCore::VertexBuffer& vertexBuffer) : VertexBuffer("?", vertexBuffer)
    {
        
    }

    VertexBuffer::VertexBuffer(const std::string& path, const VertexCore::VertexBuffer& vertexBuffer) 
        : stride((UINT)vertexBuffer.GetVertexLayout().size()), path(path), vertexLayout(vertexBuffer.GetVertexLayout())
    {
        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.ByteWidth = UINT(vertexBuffer.size());
        vertexBufferDesc.StructureByteStride = stride;

        D3D11_SUBRESOURCE_DATA initData = { };
        initData.pSysMem = vertexBuffer.data();

        HRESULT hr = GetDevice(Window::GetDxGraphic())->CreateBuffer(&vertexBufferDesc, &initData, &(this->vertexBuffer));
        Require::Check(hr, ErrorCode::GRAPHICS_BufferCreateFailed, "Vertex Buffer와 DESC 정보로 버텍스 버퍼 생성 실패");
    }

	void VertexBuffer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		const UINT offset = 0;

		Require::Check([&] { GetDeviceContext(Window::GetDxGraphic())->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset); }, ErrorCode::GRAPHICS_BindFailed, "버텍스 버퍼를 입력 어셈블러에 바인딩 실패");
	}

    const VertexCore::VertexLayout& VertexBuffer::GetVertexLayout() const noexcept
    {
        return vertexLayout;
    }

    std::shared_ptr<VertexBuffer> VertexBuffer::GetRender(const std::string& path, const VertexCore::VertexBuffer& vertexBuffer)
    {
        assert(path != "?");

        return RenderManager::GetRender<VertexBuffer>(path, vertexBuffer);
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