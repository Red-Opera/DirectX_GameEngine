#include "stdafx.h"
#include "SkyboxTransformConstantBuffer.h"


namespace Graphic
{
	SkyboxTransformConstantBuffer::SkyboxTransformConstantBuffer(UINT slot)
		: vertexConstantBuffer{ std::make_unique<VertexConstantBuffer<Transform>>(slot) }
	{

	}

	void SkyboxTransformConstantBuffer::SetRenderPipeline() NOEXCEPTRELEASE
	{
		Require::Check([&]() { UpdateRender(GetTransform()); }, ErrorCode::GRAPHICS_BindFailed, "스카이박스 변환 행렬 상수 버퍼를 버텍스 셰이더 파이프라인에 바인딩 실패");
	}

	void SkyboxTransformConstantBuffer::UpdateRender(const Transform& transform) NOEXCEPTRELEASE
	{
		vertexConstantBuffer->Update(transform);
		vertexConstantBuffer->SetRenderPipeline();
	}

	SkyboxTransformConstantBuffer::Transform SkyboxTransformConstantBuffer::GetTransform() NOEXCEPTRELEASE
	{
		return { DirectX::XMMatrixTranspose(Window::GetDxGraphic().GetCamera() * Window::GetDxGraphic().GetProjection())};
	}
}