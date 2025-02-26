#include "stdafx.h"
#include "CameraIndicator.h"

#include "Core/Draw/BaseModel/Sphere.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

CameraIndicator::CameraIndicator(DxGraphic& graphic)
{
	using namespace Graphic;

	const auto geometryTag = "$Camera";

	VertexCore::VertexLayout layout;
	layout.AddType(VertexCore::VertexLayout::Position3D);
	
	VertexCore::VertexBuffer vertices{ std::move(layout) };

	{
		const float x = 4.0f / 3.0f * 0.75f;
		const float y = 1.0f * 0.75f;
		const float z = -2.0f;
		const float thalf = x * 0.5f;
		const float tspace = y * 0.2f;

		vertices.emplace_back(DirectX::XMFLOAT3{ -x, y, 0.0f });
		vertices.emplace_back(DirectX::XMFLOAT3{ x, y, 0.0f });
		vertices.emplace_back(DirectX::XMFLOAT3{ x, -y, 0.0f });
		vertices.emplace_back(DirectX::XMFLOAT3{ -x, -y, 0.0f });
		vertices.emplace_back(DirectX::XMFLOAT3{ 0.0f, 0.0f, z });
		vertices.emplace_back(DirectX::XMFLOAT3{ -thalf, y + tspace, 0.0f });
		vertices.emplace_back(DirectX::XMFLOAT3{ thalf, y + tspace, 0.0f });
		vertices.emplace_back(DirectX::XMFLOAT3{ 0.0f, y + tspace + thalf, 0.0f });
	}

	std::vector<USHORT> indices;
	{
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(3);
		indices.push_back(0);
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(1);
		indices.push_back(4);
		indices.push_back(2);
		indices.push_back(4);
		indices.push_back(3);
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(7);
		indices.push_back(5);
	}

	vertexBuffer = VertexBuffer::GetRender(graphic, geometryTag, vertices);
	indexBuffer = IndexBuffer::GetRender(graphic, geometryTag, indices);
	primitiveTopology = PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		Technique line{ RenderingChannel::main };
		RenderStep only("lambertian");

		auto vertexShader = VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl");
		only.AddRender(InputLayout::GetRender(graphic, vertices.GetVertexLayout(), *vertexShader));
		only.AddRender(std::move(vertexShader));

		only.AddRender(PixelShader::GetRender(graphic, "Shader/ColorShader.hlsl"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 0.2f, 0.2f, 0.6f };
			float padding;
		} colorConst;

		only.AddRender(PixelConstantBuffer<PSColorConstant>::GetRender(graphic, colorConst, 1u));
		only.AddRender(std::make_shared<TransformConstantBuffer>(graphic));
		only.AddRender(Rasterizer::GetRender(graphic, false));

		line.push_back(std::move(only));
		AddTechnique(std::move(line));
	}
}

void CameraIndicator::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

void CameraIndicator::SetRotation(DirectX::XMFLOAT3 rotation) noexcept
{
	this->rotation = rotation;
}

DirectX::XMMATRIX CameraIndicator::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotation)) *
		   DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
}
