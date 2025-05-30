#include "stdafx.h"
#include "TextureCubeObject.h"

#include "../BaseModel/CubeFrame.h"

#include "Core/Component/Transform/TransformComponent.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TransformConstantBufferScaling.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

TextureCubeObject::TextureCubeObject(std::shared_ptr<class Object> object, std::string path)
	: ColorObject(object), path(path)
{

}

void TextureCubeObject::Initialize()
{
	using VertexCore::VertexLayout;
	using namespace Graphic;

	Scale scale = transform->GetLocalScale();

	auto model = CubeFrame::CreateTextureFrame();
	model.Transform(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z));
	model.SetNormalVector();

	const auto geometryTag = "TextureCube.X" + std::to_string(scale.x) + "Y" + std::to_string(scale.y) + "Z" + std::to_string(scale.z);
	vertexBuffer = VertexBuffer::GetRender(geometryTag, model.vertices);
	indexBuffer = IndexBuffer::GetRender(geometryTag, model.indices);
	primitiveTopology = PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto transformConstantBuffer = std::make_shared<TransformConstantBuffer>();

	{
		Technique tech("Cube", RenderingChannel::main);
		{
			RenderStep cubeRender("lambertian");

			cubeRender.AddRender(Texture::GetRender(path));
			cubeRender.AddRender(SamplerState::GetRender());

			auto vertexShader = VertexShader::GetRender("Shader/ShadowTest.hlsl");
			cubeRender.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *vertexShader));
			cubeRender.AddRender(std::move(vertexShader));

			cubeRender.AddRender(PixelShader::GetRender("Shader/ShadowTest.hlsl"));

			DynamicConstantBuffer::EditLayout layout;
			layout.add<DynamicConstantBuffer::float3>("specularColor");
			layout.add<DynamicConstantBuffer::float1>("specularPower");
			layout.add<DynamicConstantBuffer::float1>("specularGlass");

			auto buffer = DynamicConstantBuffer::Buffer(std::move(layout));
			buffer["specularColor"] = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
			buffer["specularPower"] = 0.1f;
			buffer["specularGlass"] = 20.0f;
			cubeRender.AddRender(std::make_shared<Graphic::CachingPixelConstantBufferEx>(buffer, 1u));

			cubeRender.AddRender(Rasterizer::GetRender(false));

			cubeRender.AddRender(transformConstantBuffer);

			tech.push_back(std::move(cubeRender));
		}

		AddTechnique(std::move(tech));
	}

	{
		Technique tech("Outline", RenderingChannel::main);
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

	// 그림자 맵 Technique
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

	ColorObject::Initialize();
}