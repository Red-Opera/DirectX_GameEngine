#include "stdafx.h"
#include "TransformConstantBuffer.h"

#include "Core/Window.h"

namespace Graphic
{
	TransformConstantBuffer::TransformConstantBuffer(UINT slot)
	{
		if (!vertexConstantBufferMatrix)
			vertexConstantBufferMatrix = std::make_unique<VertexConstantBuffer<Transform>>(slot);
	}

	void TransformConstantBuffer::InitializeParentReference(const Drawable& parent) noexcept
	{
		this->parent = &parent;
	}

	void TransformConstantBuffer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		CREATEINFOMANAGERNOHR(Window::GetDxGraphic());

		GRAPHIC_THROW_INFO_ONLY(UpdateSetRenderPipeline(GetTransform()));
	}

	void TransformConstantBuffer::UpdateSetRenderPipeline(const Transform& transform) NOEXCEPTRELEASE
	{
		assert(parent != nullptr);

		vertexConstantBufferMatrix->Update(transform);
		vertexConstantBufferMatrix->SetRenderPipeline();
	}

	TransformConstantBuffer::Transform TransformConstantBuffer::GetTransform() NOEXCEPTRELEASE
	{
		assert(parent != nullptr);

		// 부모의 Transformd에서 카메라 위치를 곱하여 View 위치를 구함
		const auto model = parent->GetTransformMatrix();
		const auto viewTransform = model * Window::GetDxGraphic().GetCamera();

		// 상수 버퍼로 만들 Transform을 만듬
		return
		{
			DirectX::XMMatrixTranspose(model),
			DirectX::XMMatrixTranspose(viewTransform),

			// World * View * Projection
			DirectX::XMMatrixTranspose(viewTransform * Window::GetDxGraphic().GetProjection())
		};
	}

	std::unique_ptr<RenderInstance> TransformConstantBuffer::Instance() const noexcept
	{
		return std::make_unique<TransformConstantBuffer>(*this);
	}

	std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transform>> TransformConstantBuffer::vertexConstantBufferMatrix;
}