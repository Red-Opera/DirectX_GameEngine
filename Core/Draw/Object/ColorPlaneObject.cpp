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
		ImGui::DragFloat("ScaleX", &scale.x, 0.1f, 0.0f, 0.0f, "%.1f");
		ImGui::DragFloat("ScaleY", &scale.y, 0.1f, 0.0f, 0.0f, "%.1f");

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
