#include "stdafx.h"
#include "ColorSphere.h"
#include "Sphere.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/Exception/GraphicsException.h"

ColorSphere::ColorSphere(DxGraphic& graphic, float radius)
{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
		};

		auto model = Sphere::Make<Vertex>();
		model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<VertexBuffer>(graphic, model.vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(graphic, model.indices));

		auto vertexShader = std::make_unique<VertexShader>(graphic, L"Shader/ColorShader2.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		AddStaticBind(std::move(vertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphic, L"Shader/ColorShader2.hlsl"));

		struct PixelShaderLightConstant
		{
			DirectX::XMVECTOR color = { 1.0f, 1.0f, 1.0f };
			float padding;
		} pixelConstant;

		AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderLightConstant>>(graphic, pixelConstant));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(graphic, inputLayoutDesc, VSShaderCode));
		AddStaticBind(std::make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	else
		SetIndexFromStatic();

	AddBind(std::make_unique<TransformConstantBuffer>(graphic, *this));
}

void ColorSphere::Update(float deltaTime) noexcept
{

}

void ColorSphere::SetPos(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

DirectX::XMMATRIX ColorSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}