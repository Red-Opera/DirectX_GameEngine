#include "stdafx.h"
#include "VertexBuffer.h"

void VertexBuffer::PipeLineSet(DxGraphic& graphic) noexcept
{
	const UINT offset = 0;

	GetDeviceContext(graphic)->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
}
