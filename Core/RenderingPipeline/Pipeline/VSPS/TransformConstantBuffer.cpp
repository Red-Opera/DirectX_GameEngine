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

		// �θ��� Transformd���� ī�޶� ��ġ�� ���Ͽ� View ��ġ�� ����
		const auto model = parent->GetTransformMatrix();
		const auto viewTransform = model * Window::GetDxGraphic().GetCamera();

		// ��� ���۷� ���� Transform�� ����
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