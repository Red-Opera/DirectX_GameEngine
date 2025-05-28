#include "stdafx.h"
#include "ModelEditor.h"

#include "Core/Draw/Model.h"

void ModelEditor::CreateWindow(Model& model)
{
	ImGui::Begin(name.c_str());
	ImGui::Columns(2, nullptr, true);
	model.Accept(*this);
	ImGui::NextColumn();

	if (selectNode != nullptr)
	{
		bool isNotMath = false;
		const auto dcheck = [&isNotMath](bool changed) {isNotMath = isNotMath || changed; };
		auto transformComponent = GetTransformComponent();
		auto tf = transformComponent->GetLocalTransform();

		Rotation toRotaion;
		toRotaion.x = tf.rotation.x * 180.0f / Math::PI;
		toRotaion.y = tf.rotation.y * 180.0f / Math::PI;
		toRotaion.z = tf.rotation.z * 180.0f / Math::PI;

		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
		dcheck(ImGui::DragFloat("X", &(tf.position.x), 0.1f, 0.0f, 0.0f, "%.1f"));
		dcheck(ImGui::DragFloat("Y", &(tf.position.y), 0.1f, 0.0f, 0.0f, "%.1f"));
		dcheck(ImGui::DragFloat("Z", &(tf.position.z), 0.1f, 0.0f, 0.0f, "%.1f"));

		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
		dcheck(ImGui::DragFloat("X Rotation", &(toRotaion.x), 0.1f, 0.0f, 0.0f, "%.1f"));
		dcheck(ImGui::DragFloat("Y Rotation", &(toRotaion.y), 0.1f, 0.0f, 0.0f, "%.1f"));
		dcheck(ImGui::DragFloat("Z Rotation", &(toRotaion.z), 0.1f, 0.0f, 0.0f, "%.1f"));

		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Scale");
		dcheck(ImGui::DragFloat("X Scale", &(tf.scale.x), 0.1f, 0.0f, 0.0f, "%.1f"));
		dcheck(ImGui::DragFloat("Y Scale", &(tf.scale.y), 0.1f, 0.0f, 0.0f, "%.1f"));
		dcheck(ImGui::DragFloat("Z Scale", &(tf.scale.z), 0.1f, 0.0f, 0.0f, "%.1f"));

		if (isNotMath)
		{
			toRotaion.x = Math::NormalizeAngle(toRotaion.x);
			toRotaion.y = Math::NormalizeAngle(toRotaion.y);
			toRotaion.z = Math::NormalizeAngle(toRotaion.z);

			toRotaion.x = Math::ConvertAngleToRadian(toRotaion.x);
			toRotaion.y = Math::ConvertAngleToRadian(toRotaion.y);
			toRotaion.z = Math::ConvertAngleToRadian(toRotaion.z);

			selectNode->ApplyWorldTransform(
				DirectX::XMMatrixScaling(tf.scale.x, tf.scale.y, tf.scale.z) *
				DirectX::XMMatrixRotationRollPitchYaw(toRotaion.x, toRotaion.y, toRotaion.z) *
				DirectX::XMMatrixTranslation(tf.position.x, tf.position.y, tf.position.z)
			);

			transformComponent->SetPosition(tf.position);
			transformComponent->SetRotation(toRotaion);
			transformComponent->SetScale(tf.scale);
		}

		TechniqueEditor probe;
		selectNode->Accept(probe);
	}

	ImGui::End();
}