#include "stdafx.h"
#include "ColorObject.h"

#include "Core/App.h"
#include "Core/Component/TransformComponent.h"
#include "Core/Draw/Base/TriangleIndexList.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TransformConstantBufferScaling.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"
#include "Utility/MathInfo.h"

#include "External/Imgui/imgui.h"

ColorObject::ColorObject(std::shared_ptr<class Object> object)
	: Component(object)
{

}

void ColorObject::SetRenderingPipeline(GraphicResource::Image::Color color, bool isLit, class TriangleIndexList model)
{
	using VertexCore::VertexLayout;
	using namespace Graphic;

	Scale scale = transform->GetLocalScale();

	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f));

	const auto geometryTag = "$ColorCone.X" + std::to_string(scale.x) + "Y" + std::to_string(scale.y) + "Z" + std::to_string(scale.z);
	vertexBuffer = VertexBuffer::GetRender(geometryTag, model.vertices);
	indexBuffer = IndexBuffer::GetRender(geometryTag, model.indices);
	primitiveTopology = PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto transformConstantBuffer = std::make_shared<TransformConstantBuffer>();

	{
		Technique tech("Color Cone", RenderingChannel::main);

		{
			RenderStep coneRender("lambertian");

			coneRender.AddRender(SamplerState::GetRender());

			if (isLit)
			{
				auto vertexShader = VertexShader::GetRender("Shader/LitColor.hlsl");
				coneRender.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *vertexShader));
				coneRender.AddRender(std::move(vertexShader));

				coneRender.AddRender(PixelShader::GetRender("Shader/LitColor.hlsl"));
			}

			else
			{
				auto vertexShader = VertexShader::GetRender("Shader/ColorShader.hlsl");
				coneRender.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *vertexShader));
				coneRender.AddRender(std::move(vertexShader));

				coneRender.AddRender(PixelShader::GetRender("Shader/ColorShader.hlsl"));
			}

			DynamicConstantBuffer::EditLayout layout;
			layout.add<DynamicConstantBuffer::float3>("materialColor");

			if (isLit)
			{
				layout.add<DynamicConstantBuffer::float3>("specularColor");
				layout.add<DynamicConstantBuffer::float1>("specularPower");
				layout.add<DynamicConstantBuffer::float1>("specularGlass");
			}

			auto buffer = DynamicConstantBuffer::Buffer(std::move(layout));
			buffer["materialColor"] = DirectX::XMFLOAT3(color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f);

			if (isLit)
			{
				buffer["specularColor"] = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
				buffer["specularPower"] = 0.1f;
				buffer["specularGlass"] = 20.0f;
			}

			coneRender.AddRender(std::make_shared<Graphic::CachingPixelConstantBufferEx>(buffer, 1u));

			coneRender.AddRender(Rasterizer::GetRender(false));

			coneRender.AddRender(transformConstantBuffer);

			tech.push_back(std::move(coneRender));
		}

		AddTechnique(std::move(tech));
	}

	{
		Technique tech("Outline", RenderingChannel::main, false);
		{
			RenderStep outlineMask("outlineMask");

			outlineMask.AddRender(
				InputLayout::GetRender(
					model.vertices.GetVertexLayout(),
					*VertexShader::GetRender("Shader/ColorShader.hlsl")
				)
			);

			outlineMask.AddRender(std::move(transformConstantBuffer));

			tech.push_back(std::move(outlineMask));
		}

		{
			RenderStep outlineDraw("outlineDraw");

			DynamicConstantBuffer::EditLayout layout;
			layout.add<DynamicConstantBuffer::float4>("color");

			auto buffer = DynamicConstantBuffer::Buffer(std::move(layout));
			buffer["color"] = DirectX::XMFLOAT4(1.0f, 0.4f, 0.4f, 1.0f);
			outlineDraw.AddRender(std::make_shared<Graphic::CachingPixelConstantBufferEx>(buffer, 1u));

			outlineDraw.AddRender(
				InputLayout::GetRender(
					model.vertices.GetVertexLayout(),
					*VertexShader::GetRender("Shader/ColorShader.hlsl")
				));

			outlineDraw.AddRender(std::make_shared<TransformConstantBuffer>());

			tech.push_back(std::move(outlineDraw));
		}

		AddTechnique(std::move(tech));
	}

	// ±×¸²ÀÚ ¸Ê Technique
	if (isLit)
	{
		Technique shadowMap{ "ShadowMap", RenderingChannel::shadow, true };
		{
			RenderStep draw("ShadowMap");

			draw.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *VertexShader::GetRender("Shader/ColorShader.hlsl")));
			draw.AddRender(std::make_shared<TransformConstantBuffer>());

			shadowMap.push_back(std::move(draw));
		}

		AddTechnique(std::move(shadowMap));
	}
}

DirectX::XMMATRIX ColorObject::GetTransformMatrix() const noexcept
{
	const Position position = transform->GetPosition();
	const Rotation rotation = transform->GetLocalRotation();
	const Scale scale = transform->GetLocalScale();

	return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void ColorObject::SetColor(GraphicResource::Image::Color color)
{
	this->color = color;
}

GraphicResource::Image::Color ColorObject::GetColor() const
{
	return color;
}

void ColorObject::SetLit(bool isLit)
{
	this->isLit = isLit;
}

bool ColorObject::GetLit() const
{
	return isLit;
}

void ColorObject::Initialize()
{
	LinkTechniques(App::GetRenderGraph());
}

void ColorObject::Update()
{
	Component::Update();

	Submit(RenderingChannel::main);

	Submit(RenderingChannel::shadow);
}

void ColorObject::LateUpdate()
{
	Component::LateUpdate();
}