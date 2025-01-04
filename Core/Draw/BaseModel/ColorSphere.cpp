#include "stdafx.h"
#include "ColorSphere.h"
#include "Sphere.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/Exception/GraphicsException.h"

ColorSphere::ColorSphere(DxGraphic& graphic, float radius)
{
	using namespace Graphic;

	auto model = Sphere::Make();
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	const auto objectTag = "$sphere." + std::to_string(radius);
	AddRender(std::make_shared<VertexBuffer>(graphic, objectTag, model.vertices));
	AddRender(std::make_shared<IndexBuffer>(graphic, objectTag, model.indices));

	auto vertexShader = VertexShader::GetRender(graphic, "Shader/ColorShader2.hlsl");
	auto VSShaderCode = vertexShader->GetShaderCode();
	AddRender(std::move(vertexShader));

	AddRender(PixelShader::GetRender(graphic, "Shader/ColorShader2.hlsl"));

	struct PixelShaderLightConstant
	{
		DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f };
		float padding;
	} pixelConstant;

	AddRender(PixelConstantBuffer<PixelShaderLightConstant>::GetRender(graphic, pixelConstant, 1u));
				
	AddRender(InputLayout::GetRender(graphic, model.vertices.GetVertexLayout(), VSShaderCode));
	AddRender(PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddRender(std::make_shared<TransformConstantBuffer>(graphic, *this));

	AddRender(ColorBlend::GetRender(graphic, false));
	AddRender(Rasterizer::GetRender(graphic, false));
}

void ColorSphere::SetPos(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

DirectX::XMMATRIX ColorSphere::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}