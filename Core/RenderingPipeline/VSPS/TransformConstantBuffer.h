#pragma once
#include "../Bindable.h"

#include "ConstantBuffer.h"
#include "Core/Draw/Base/Drawable.h"

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(DxGraphic& graphic, const Drawable& parent, UINT slot = 0u);
	
	// Bindable을(를) 통해 상속됨
	void PipeLineSet(DxGraphic& graphic) noexcept override;

private:
	struct Transform
	{
		DirectX::XMMATRIX worldViewProjection;
		DirectX::XMMATRIX transform;
	};

	static std::unique_ptr<VertexConstantBuffer<Transform>> vertexConstantBufferMatrix;
	const Drawable& parent;
};

