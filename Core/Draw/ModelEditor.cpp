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

        // 쿼터니언을 오일러 각도로 변환
        Vector3 eulerAngles = Vector::ConvertEuler(tf.rotation);

        // 라디안을 도로 변환
        Vector3 toRotation;
        toRotation.x = eulerAngles.x * 180.0f / Math::PI;
        toRotation.y = eulerAngles.y * 180.0f / Math::PI;
        toRotation.z = eulerAngles.z * 180.0f / Math::PI;

        ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
        dcheck(ImGui::DragFloat("X", &(tf.position.x), 0.1f, 0.0f, 0.0f, "%.1f"));
        dcheck(ImGui::DragFloat("Y", &(tf.position.y), 0.1f, 0.0f, 0.0f, "%.1f"));
        dcheck(ImGui::DragFloat("Z", &(tf.position.z), 0.1f, 0.0f, 0.0f, "%.1f"));

        ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
        dcheck(ImGui::DragFloat("X Rotation", &(toRotation.x), 0.1f, 0.0f, 0.0f, "%.1f"));
        dcheck(ImGui::DragFloat("Y Rotation", &(toRotation.y), 0.1f, 0.0f, 0.0f, "%.1f"));
        dcheck(ImGui::DragFloat("Z Rotation", &(toRotation.z), 0.1f, 0.0f, 0.0f, "%.1f"));

        ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Scale");
        dcheck(ImGui::DragFloat("X Scale", &(tf.scale.x), 0.1f, 0.0f, 0.0f, "%.1f"));
        dcheck(ImGui::DragFloat("Y Scale", &(tf.scale.y), 0.1f, 0.0f, 0.0f, "%.1f"));
        dcheck(ImGui::DragFloat("Z Scale", &(tf.scale.z), 0.1f, 0.0f, 0.0f, "%.1f"));

        if (isNotMath)
        {
            // 각도 정규화
            toRotation.x = Math::NormalizeAngle(toRotation.x);
            toRotation.y = Math::NormalizeAngle(toRotation.y);
            toRotation.z = Math::NormalizeAngle(toRotation.z);

            // 도를 라디안으로 변환
            Euler eulerRadians;
            eulerRadians.x = Math::ConvertAngleToRadian(toRotation.x);
            eulerRadians.y = Math::ConvertAngleToRadian(toRotation.y);
            eulerRadians.z = Math::ConvertAngleToRadian(toRotation.z);

            // 오일러 각도를 쿼터니언으로 변환
            Quaternion newRotation = Vector::ConvertQuaternion(eulerRadians);

            // 변환 행렬 적용
            selectNode->ApplyWorldTransform(
                DirectX::XMMatrixScaling(tf.scale.x, tf.scale.y, tf.scale.z) *
                DirectX::XMMatrixRotationRollPitchYaw(eulerRadians.x, eulerRadians.y, eulerRadians.z) *
                DirectX::XMMatrixTranslation(tf.position.x, tf.position.y, tf.position.z)
            );

            // TransformComponent 업데이트
            transformComponent->SetLocalPosition(tf.position);
            transformComponent->SetLocalRotation(newRotation);
            transformComponent->SetLocalScale(tf.scale);
        }

        TechniqueEditor probe;
        selectNode->Accept(probe);
    }

    ImGui::End();
}