#include "stdafx.h"
#include "PrimitiveTopology.h"

PrimitiveTopology::PrimitiveTopology(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology) : topology(topology)
{

}

void PrimitiveTopology::PipeLineSet(DxGraphic& grahpic) noexcept
{
	GetDeviceContext(grahpic)->IASetPrimitiveTopology(topology);
}