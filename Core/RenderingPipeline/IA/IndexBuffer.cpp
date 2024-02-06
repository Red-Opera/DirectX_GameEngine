#include "stdafx.h"
#include "IndexBuffer.h"

#include "Core/Exception/GraphicsException.h"

IndexBuffer::IndexBuffer(DxGraphic& graphic, const vector<unsigned short>& indices) : indexCount((UINT) indices.size())
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

	GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateBuffer(&indexBufferDesc, &initData, &indexBuffer));
}

void IndexBuffer::PipeLineSet(DxGraphic& graphic) noexcept
{
	GetDeviceContext(graphic)->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}

UINT IndexBuffer::GetIndexCount() const noexcept
{
	return indexCount;
}
