#include "stdafx.h"
#include "CameraFrustum.h"

#include "Core/Draw/BaseModel/Sphere.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

CameraFrustum::CameraFrustum(DxGraphic& graphic, float width, float height, float nearZ, float farZ)
{
	using namespace Graphic;

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
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(7);
		indices.push_back(4);
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(1);
		indices.push_back(5);
		indices.push_back(2);
		indices.push_back(6);
		indices.push_back(3);
		indices.push_back(7);
	};

	SetVertices(graphic, width, height, nearZ, farZ);
	indexBuffer = IndexBuffer::GetRender(graphic, "$Frustum", indices);
	primitiveTopology = PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		Technique line;
		{
			RenderStep notOccluded("lambertian");

			auto vertexShader = VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl");
			notOccluded.AddRender(InputLayout::GetRender(graphic, vertexBuffer->GetVertexLayout(), *vertexShader));
			notOccluded.AddRender(std::move(vertexShader));

			notOccluded.AddRender(PixelShader::GetRender(graphic, "Shader/ColorShader.hlsl"));

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 0.6f, 0.2f, 0.2f };
				float padding;
			} colorConst;

			notOccluded.AddRender(PixelConstantBuffer<PSColorConstant>::GetRender(graphic, colorConst, 1u));
			notOccluded.AddRender(std::make_shared<TransformConstantBuffer>(graphic));
			notOccluded.AddRender(Rasterizer::GetRender(graphic, false));

			line.push_back(std::move(notOccluded));
		}

		{
			RenderStep occluded("wireframe");

			auto vertexShader = VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl");
			occluded.AddRender(InputLayout::GetRender(graphic, vertexBuffer->GetVertexLayout(), *vertexShader));
			occluded.AddRender(std::move(vertexShader));

			occluded.AddRender(PixelShader::GetRender(graphic, "Shader/ColorShader.hlsl"));

			struct PSColorConstant
			{
				DirectX::XMFLOAT3 color = { 0.25f, 0.08f, 0.08f };
				float padding;
			} colorConst;

			occluded.AddRender(PixelConstantBuffer<PSColorConstant>::GetRender(graphic, colorConst, 1u));
			occluded.AddRender(std::make_shared<TransformConstantBuffer>(graphic));
			occluded.AddRender(Rasterizer::GetRender(graphic, false));

			line.push_back(std::move(occluded));
		}

		AddTechnique(std::move(line));
	}
}

void CameraFrustum::SetVertices(DxGraphic& graphic, float width, float height, float nearZ, float farZ)
{
	VertexCore::VertexLayout layout;
	layout.AddType(VertexCore::VertexLayout::Position3D);

	VertexCore::VertexBuffer vertices{ std::move(layout) };

	{
		const float zRatio = farZ / nearZ;
		const float nearX = width / 2.0f;
		const float nearY = height / 2.0f;
		const float farX = nearX * zRatio;
		const float farY = nearY * zRatio;


		vertices.emplace_back(DirectX::XMFLOAT3{ -nearX, nearY, nearZ });
		vertices.emplace_back(DirectX::XMFLOAT3{  nearX, nearY, nearZ });
		vertices.emplace_back(DirectX::XMFLOAT3{  nearX,-nearY, nearZ });
		vertices.emplace_back(DirectX::XMFLOAT3{ -nearX,-nearY, nearZ });
		vertices.emplace_back(DirectX::XMFLOAT3{ -farX, farY, farZ });
		vertices.emplace_back(DirectX::XMFLOAT3{  farX, farY, farZ });
		vertices.emplace_back(DirectX::XMFLOAT3{  farX,-farY, farZ });
		vertices.emplace_back(DirectX::XMFLOAT3{ -farX,-farY, farZ });
	}

	vertexBuffer = std::make_shared<Graphic::VertexBuffer>(graphic, vertices);
}

void CameraFrustum::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

void CameraFrustum::SetRotation(DirectX::XMFLOAT3 rotation) noexcept
{
	this->rotation = rotation;
}

DirectX::XMMATRIX CameraFrustum::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotation)) *
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
}