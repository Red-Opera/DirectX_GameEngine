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

		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
		dcheck(ImGui::SliderFloat("X", &(tf.position.x), -60.f, 60.f));
		dcheck(ImGui::SliderFloat("Y", &(tf.position.y), -60.f, 60.f));
		dcheck(ImGui::SliderFloat("Z", &(tf.position.z), -60.f, 60.f));
		ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
		dcheck(ImGui::SliderAngle("X Rotation", &(tf.rotation.x), -180.0f, 180.0f));
		dcheck(ImGui::SliderAngle("Y Rotation", &(tf.rotation.y), -180.0f, 180.0f));
		dcheck(ImGui::SliderAngle("Z Rotation", &(tf.rotation.z), -180.0f, 180.0f));

		if (isNotMath)
		{
			selectNode->ApplyWorldTranfsorm(
				DirectX::XMMatrixRotationX(tf.rotation.x) *
				DirectX::XMMatrixRotationY(tf.rotation.y) *
				DirectX::XMMatrixRotationZ(tf.rotation.z) *
				DirectX::XMMatrixTranslation(tf.position.x, tf.position.y, tf.position.z)
			);

			transformComponent->SetPosition(tf.position);
			transformComponent->SetRotation(tf.rotation);
		}

		TechniqueEditor probe;
		selectNode->Accept(probe);
	}

	ImGui::End();
}