#include "stdafx.h"
#include "ColorPlaneObject.h"

#include "../BaseModel/ColorPlaneFrame.h"

#include "Core/Object/Object.h"
#include "Core/Component/TransformComponent.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/TechniqueBase.h"

ColorPlaneObject::ColorPlaneObject(std::shared_ptr<Object> object)
	: ColorObject(object)
{

}

void ColorPlaneObject::Initialize()
{
	TriangleIndexList model = isLit ? ColorPlaneFrame::CreateTextureFrame() : ColorPlaneFrame::CreateFrame();

	SetRenderingPipeline(color, isLit, model);

	ColorObject::Initialize();
}

void ColorPlaneObject::CreateControlWindow(const char* name) noexcept
{
	Position& position = GetObject()->transform->GetLocalPosition();
	Rotation& rotation = GetObject()->transform->GetLocalRotation();
	Scale& scale = GetObject()->transform->GetLocalScale();

	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("PositionX", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("PositionY", &position.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("PositionZ", &position.z, -80.0f, 80.0f, "%.1f");

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &rotation.x, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &rotation.y, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &rotation.z, -180.0f, 180.0f);

		ImGui::Text("Scale");
		ImGui::SliderFloat("ScaleX", &scale.x, 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("ScaleY", &scale.y, 0.1f, 10.0f, "%.1f");

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
