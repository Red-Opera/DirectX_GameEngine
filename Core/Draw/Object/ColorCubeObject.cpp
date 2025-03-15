#include "stdafx.h"
#include "ColorCubeObject.h"

#include "../BaseModel/CubeFrame.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TransformConstantBufferScaling.h"
#include "Core/RenderingPipeline/RenderingChannel.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

#include "External/Imgui/imgui.h"

ColorCubeObject::ColorCubeObject(float size, GraphicResource::Image::Color color, bool isLit) : size(size)
{
	using VertexCore::VertexLayout;
	using namespace Graphic;

	auto model = CubeFrame::CreateTextureFrame();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));

	if (isLit)
		model.SetNormalVector();

	const auto geometryTag = "$ColorCube." + std::to_string(size);
	vertexBuffer = VertexBuffer::GetRender(geometryTag, model.vertices);
	indexBuffer = IndexBuffer::GetRender(geometryTag, model.indices);
	primitiveTopology = PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto transformConstantBuffer = std::make_shared<TransformConstantBuffer>();

	{
		Technique tech("Color Cube", RenderingChannel::main);

		{
			RenderStep cubeRender("lambertian");

			cubeRender.AddRender(SamplerState::GetRender());

			if (isLit)
			{
				auto vertexShader = VertexShader::GetRender("Shader/LitColor.hlsl");
				cubeRender.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *vertexShader));
				cubeRender.AddRender(std::move(vertexShader));

				cubeRender.AddRender(PixelShader::GetRender("Shader/LitColor.hlsl"));
			}

			else
			{
				auto vertexShader = VertexShader::GetRender("Shader/ColorShader.hlsl");
				cubeRender.AddRender(InputLayout::GetRender(model.vertices.GetVertexLayout(), *vertexShader));
				cubeRender.AddRender(std::move(vertexShader));

				cubeRender.AddRender(PixelShader::GetRender("Shader/ColorShader.hlsl"));
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

void ColorCubeObject::SetPosition(Vector3 position) noexcept
{
	this->position = position;
}

void ColorCubeObject::SetRotation(Rotation rotation) noexcept
{
	this->rotation = rotation;
}

DirectX::XMMATRIX ColorCubeObject::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void ColorCubeObject::CreateControlWindow(const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &rotation.x, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &rotation.y, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &rotation.z, -180.0f, 180.0f);

		class Probe : public TechniqueBase
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, technique->GetName().c_str());

				bool isActive = technique->GetAcive();
				ImGui::Checkbox(("Tech Active##"s + std::to_string(techIndex)).c_str(), &isActive);

				technique->SetActive(isActive);
			}

			bool OnVisitBuffer(DynamicConstantBuffer::Buffer& buffer) override
			{
				float isNotMatch = false;
				const auto IsNotMatch = [&isNotMatch](bool changed) { isNotMatch = isNotMatch || changed; };
				auto Tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufferIndex)](const char* label) mutable
					{
						tagScratch = label + tagString;

						return tagScratch.c_str();
					};

				if (auto data = buffer["materialColor"]; data.IsExist())
					IsNotMatch(ImGui::ColorEdit3(Tag("Material Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(data))));

				if (auto data = buffer["scale"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Scale"), &data, 1.0f, 2.0f, "%.3f"));

				if (auto data = buffer["color"]; data.IsExist())
					IsNotMatch(ImGui::ColorPicker4(Tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(data))));

				if (auto data = buffer["specularIntensity"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Specular Intensity"), &data, 0.0f, 1.0f));

				if (auto data = buffer["specularPower"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Glassiness"), &data, 0.0f, 1.0f, "%.1f"));

				return isNotMatch;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
}