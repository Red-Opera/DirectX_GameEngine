#include "stdafx.h"
#include "TestPlane.h"

#include "BaseModel/Plane.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Utility/Imgui/imgui.h"

TestPlane::TestPlane(DxGraphic& graphic, float size, DirectX::XMFLOAT4 color) : matConst({ color })
{
	//using VertexCore::VertexLayout;
	//using namespace Graphic;
	//
	//auto model = Plane::Make();
	//model.Transform(DirectX::XMMatrixScaling(size, size, 1.0f));
	//
	//const auto geometryTag = "$plane." + std::to_string(size);
	//AddRender(VertexBuffer::GetRender(graphic, geometryTag, model.vertices));
	//AddRender(IndexBuffer::GetRender(graphic, geometryTag, model.indices));
	//
	//auto vertexShader = VertexShader::GetRender(graphic, "Shader/ColorShader2.hlsl");
	//auto VSShaderCode = vertexShader->GetShaderCode();
	//AddRender(std::move(vertexShader));
	//
	//AddRender(PixelShader::GetRender(graphic, "Shader/ColorShader2.hlsl"));
	//AddRender(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(graphic, matConst, 1u));
	//
	//AddRender(InputLayout::GetRender(graphic, model.vertices.GetVertexLayout(), VSShaderCode));
	//
	//AddRender(PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	//
	//AddRender(std::make_shared<TransformConstantBuffer>(graphic, *this, 0u));
	//
	//AddRender(ColorBlend::GetRender(graphic, true, 0.5f));
	//AddRender(Rasterizer::GetRender(graphic, true));
}

void TestPlane::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) * 
		   DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void TestPlane::SpawnControlWindow(DxGraphic& graphic, const std::string& name) noexcept
{
	//if (ImGui::Begin(name.c_str()))
	//{
	//	ImGui::Text("Position");
	//	ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
	//	ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
	//	ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");
	//	ImGui::Text("Orientation");
	//	ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
	//	ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
	//	ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	//	ImGui::Text("Shading");
	//	
	//	auto colorBlend = GetRenderObject<Graphic::ColorBlend>();
	//	float transparency = colorBlend->GetTransparency();
	//	ImGui::SliderFloat("Transparency", &transparency, 0.0f, 1.0f);
	//	colorBlend->SetTransparency(transparency);
	//}
	//ImGui::End();
}