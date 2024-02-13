#pragma once
#include "../Bindable.h"

#include "ConstantBuffer.h"
#include "Core/Draw/Base/Drawable.h"

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(DxGraphic& graphic, const Drawable& parent);
	
	// Bindable��(��) ���� ��ӵ�
	void PipeLineSet(DxGraphic& graphic) noexcept override;

private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> vertexConstantBufferMatrix;
	const Drawable& parent;
};

