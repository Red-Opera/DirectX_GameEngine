#include "stdafx.h"
#include "ColorCube.h"
#include "Cube.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TransformConstantBufferScaling.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

#include "Utility/Imgui/imgui.h"

ColorCube::ColorCube(DxGraphic& graphic, float size)
{
	using VertexCore::VertexLayout;
	using namespace Graphic;

	auto model = Cube::CreateTextureCube();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalVector();

	const auto geometryTag = "$cube." + std::to_string(size);
	vertexBuffer = VertexBuffer::GetRender(graphic, geometryTag, model.vertices);
	indexBuffer = IndexBuffer::GetRender(graphic, geometryTag, model.indices);
	primitiveTopology = PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto transformConstantBuffer = std::make_shared<TransformConstantBuffer>(graphic);

	{
		Technique tech("Cube");
		{
			RenderStep cubeRender("lambertian");

			cubeRender.AddRender(Texture::GetRender(graphic, "Images/brickwall.jpg"));
			cubeRender.AddRender(SamplerState::GetRender(graphic));

			auto vertexShader = VertexShader::GetRender(graphic, "Shader/LitTextureDiffuse.hlsl");
			cubeRender.AddRender(InputLayout::GetRender(graphic, model.vertices.GetVertexLayout(), *vertexShader));
			cubeRender.AddRender(std::move(vertexShader));

			cubeRender.AddRender(PixelShader::GetRender(graphic, "Shader/LitTextureDiffuse.hlsl"));

			DynamicConstantBuffer::EditLayout layout;
			layout.add<DynamicConstantBuffer::float3>("specularColor");
			layout.add<DynamicConstantBuffer::float1>("specularPower");
			layout.add<DynamicConstantBuffer::float1>("specularGlass");

			auto buffer = DynamicConstantBuffer::Buffer(std::move(layout));
			buffer["specularColor"] = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
			buffer["specularPower"] = 0.1f;
			buffer["specularGlass"] = 20.0f;
			cubeRender.AddRender(std::make_shared<Graphic::CachingPixelConstantBufferEx>(graphic, buffer, 1u));

			cubeRender.AddRender(Rasterizer::GetRender(graphic, false));

			cubeRender.AddRender(transformConstantBuffer);


			tech.push_back(std::move(cubeRender));
		}

		AddTechnique(std::move(tech));
	}

	{
		Technique tech("Outline");
		{
			RenderStep outlineMask("outlineMask");

			outlineMask.AddRender(
				InputLayout::GetRender(
					graphic,
					model.vertices.GetVertexLayout(), 
					*VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl")
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
			outlineDraw.AddRender(std::make_shared<Graphic::CachingPixelConstantBufferEx>(graphic, buffer, 1u));
	
			outlineDraw.AddRender(
				InputLayout::GetRender(
					graphic,
					model.vertices.GetVertexLayout(),
					*VertexShader::GetRender(graphic, "Shader/ColorShader.hlsl")
			));
	
			outlineDraw.AddRender(std::make_shared<TransformConstantBuffer>(graphic));
			
			tech.push_back(std::move(outlineDraw));
		}
	
		AddTechnique(std::move(tech));
	}
}

void ColorCube::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

void ColorCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX ColorCube::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void ColorCube::SpawnControlWindow(DxGraphic& graphic, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

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

				if (auto data = buffer["scale"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Scale"), &data, 1.0f, 2.0f, "%.3f", 3.5f));

				if (auto data = buffer["color"]; data.IsExist())
					IsNotMatch(ImGui::ColorPicker4(Tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(data))));

				if (auto data = buffer["specularIntensity"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Specular Intensity"), &data, 0.0f, 1.0f));

				if (auto data = buffer["specularPower"]; data.IsExist())
					IsNotMatch(ImGui::SliderFloat(Tag("Glassiness"), &data, 0.0f, 1.0f, "%.1f", 1.5f));

				return isNotMatch;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
}