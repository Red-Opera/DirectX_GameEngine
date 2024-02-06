#pragma once

#include "../Bindable.h"

class PrimitiveTopology : public Bindable
{
public:
	PrimitiveTopology(DxGraphic& graphic, D3D11_PRIMITIVE_TOPOLOGY topology);
	
	void PipeLineSet(DxGraphic& grahpic) noexcept override;

protected:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};