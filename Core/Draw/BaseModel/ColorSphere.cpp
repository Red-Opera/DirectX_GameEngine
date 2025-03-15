#include "stdafx.h"
#include "ColorSphere.h"
#include "SphereFrame.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/Exception/GraphicsException.h"


ColorSphere::ColorSphere(float radius)
{
	using namespace Graphic;

	auto model = SphereFrame::CreateFrame();
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	const auto objectTag = "$sphere." + std::to_string(radius);
	vertexBuffer = std::make_shared<VertexBuffer>(objectTag, model.vertices);
	indexBuffer = std::make_shared<IndexBuffer>(objectTag, model.indices);
	primitiveTopology = PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique tech{ RenderingChannel::main };
		RenderStep sphereRender("lambertian");

		auto vertexShader = VertexShader::GetRender("Shader/ColorShader.hlsl");
		sphereRender.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *vertexShader));
		sphereRender.AddRender(std::move(vertexShader));

		sphereRender.AddRender(PixelShader::GetRender("Shader/ColorShader.hlsl"));

		struct PixelShaderLightConstant
		{
			DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f };
			float padding;
		} pixelConstant;

		sphereRender.AddRender(PixelConstantBuffer<PixelShaderLightConstant>::GetRender(pixelConstant, 1u));

		sphereRender.AddRender(std::make_shared<TransformConstantBuffer>());
		
		sphereRender.AddRender(Rasterizer::GetRender(false));

		tech.push_back(std::move(sphereRender));
		AddTechnique(std::move(tech));
	}
}

void ColorSphere::SetPos(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

DirectX::XMMATRIX ColorSphere::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}