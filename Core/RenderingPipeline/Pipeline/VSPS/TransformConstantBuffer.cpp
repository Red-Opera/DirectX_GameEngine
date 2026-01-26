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
		Require::Check([&]() { UpdateSetRenderPipeline(GetTransform()); }, ErrorCode::GRAPHICS_BindFailed, "변환 행렬 상수 버퍼를 버텍스 셰이더 파이프라인에 바인딩 실패");
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

		// 부모의 Transform에서 카메라 위치를 곱하여 View 위치를 구함
		const XMMATRIX model = parent->GetTransformMatrix();
		const XMMATRIX camera = Window::GetDxGraphic().GetCamera();
		const XMMATRIX projection = Window::GetDxGraphic().GetProjection();

		// 변환이 변경되었는지 여부를 확인
		bool isChange = !Matrix::Equal(model, beforeModel) ||
						!Matrix::Equal(camera, beforeCamera) ||
						!Matrix::Equal(projection, beforeProjection);

		if (isChange)
		{
			beforeModel = model;
			beforeCamera = camera;
			beforeProjection = projection;

			const XMMATRIX viewTransform = model * camera;

			// 상수 버퍼로 만들 Transform을 만듬
			beforeTransform =
			{
				DirectX::XMMatrixTranspose(model),
				DirectX::XMMatrixTranspose(viewTransform),

				// World * View * Projection
				DirectX::XMMatrixTranspose(viewTransform * projection)
			};
		}

		return beforeTransform;
	}

	std::unique_ptr<RenderInstance> TransformConstantBuffer::Instance() const noexcept
	{
		return std::make_unique<TransformConstantBuffer>(*this);
	}

	std::unique_ptr<VertexConstantBuffer<TransformConstantBuffer::Transform>> TransformConstantBuffer::vertexConstantBufferMatrix;
}