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

	model.Transform(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z));

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

DirectX::XMMATRIX ColorObject::GetTransformMatrix() const noexcept
{
	const Position position = transform->GetPosition();
	const Rotation rotation = transform->GetLocalRotation();
	const Scale scale = transform->GetLocalScale();

	return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
		DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void ColorObject::CreateControlWindow(const char* name) noexcept
{
	Position& position = transform->GetPosition();
	Rotation& rotation = transform->GetLocalRotation();
	Scale& scale = transform->GetLocalScale();

	if (ImGui::Begin(name))
	{
		bool positionChanged = false;
		bool rotationChanged = false;
		bool scaleChanged = false;

		ImGui::Text("Position");
		positionChanged |= ImGui::DragFloat("PositionX", &position.x, 0.1f, 0.0f, 0.0f, "%.1f");
		positionChanged |= ImGui::DragFloat("PositionY", &position.y, 0.1f, 0.0f, 0.0f, "%.1f");
		positionChanged |= ImGui::DragFloat("PositionZ", &position.z, 0.1f, 0.0f, 0.0f, "%.1f");

		if (positionChanged)
			transform->SetPosition(position);

		Rotation toRotation;
		toRotation.x = rotation.x * 180.0f / Math::PI;
		toRotation.y = rotation.y * 180.0f / Math::PI;
		toRotation.z = rotation.z * 180.0f / Math::PI;

		ImGui::Text("Orientation");
		rotationChanged |= ImGui::DragFloat("Roll", &toRotation.x, 0.1f, 0.0f, 0.0f, "%.1f");
		rotationChanged |= ImGui::DragFloat("Pitch", &toRotation.y, 0.1f, 0.0f, 0.0f, "%.1f");
		rotationChanged |= ImGui::DragFloat("Yaw", &toRotation.z, 0.1f, 0.0f, 0.0f, "%.1f");

		if (rotationChanged)
		{
			toRotation.x = Math::NormalizeAngle(toRotation.x);
			toRotation.y = Math::NormalizeAngle(toRotation.y);
			toRotation.z = Math::NormalizeAngle(toRotation.z);

			toRotation.x = Math::ConvertAngleToRadian(toRotation.x);
			toRotation.y = Math::ConvertAngleToRadian(toRotation.y);
			toRotation.z = Math::ConvertAngleToRadian(toRotation.z);

			transform->SetLocalRotation(toRotation);
		}

		ImGui::Text("Scale");
		scaleChanged |= ImGui::DragFloat("ScaleX", &scale.x, 0.1f, 0.0f, 0.0f, "%.1f");
		scaleChanged |= ImGui::DragFloat("ScaleY", &scale.y, 0.1f, 0.0f, 0.0f, "%.1f");
		scaleChanged |= ImGui::DragFloat("ScaleZ", &scale.z, 0.1f, 0.0f, 0.0f, "%.1f");

		if (scaleChanged)
			transform->SetLocalScale(scale);

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

	CreateControlWindow(GetObject()->GetName().c_str());
}