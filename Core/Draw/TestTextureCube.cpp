#include "stdafx.h"
#include "TestTextureCube.h"

#include "BaseModel/Cube.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/TransformConstantBufferAddPixel.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Utility/Imgui/imgui.h"

TestTextureCube::TestTextureCube(DxGraphic& graphic, float size)
{
	using VertexCore::VertexLayout;
	using namespace Graphic;

	auto model = Cube::SetCubeTexturePosition();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalVector();

	const auto geometryTag = "$cube." + std::to_string(size);
	vertexBuffer = VertexBuffer::GetRender(graphic, geometryTag, model.vertices);
	indexBuffer = IndexBuffer::GetRender(graphic, geometryTag, model.indices);
	primitiveTopology = PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	{
		Technique tech;

		RenderStep cubeRender;
	}

	AddRender(Texture::GetRender(graphic, "Images/brickwall.jpg"));
	AddRender(Texture::GetRender(graphic, "Images/brickwall_normal.jpg", 1u));

	auto vertexShader = VertexShader::GetRender(graphic, "Shader/TextureNormalMap.hlsl");
	auto VSShaderCode = vertexShader->GetShaderCode();
	AddRender(std::move(vertexShader));

	AddRender(PixelShader::GetRender(graphic, "Shader/TextureNormalMap.hlsl"));
	AddRender(PixelConstantBuffer<PSMaterialConstant>::GetRender(graphic, matConst, 1u));

	AddRender(InputLayout::GetRender(graphic, model.vertices.GetVertexLayout(), VSShaderCode));

	AddRender(std::make_shared<TransformConstantBufferAddPixel>(graphic, *this, 0u, 2u));
}

void TestTextureCube::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	this->position = position;
}

void TestTextureCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestTextureCube::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void TestTextureCube::SpawnControlWindow(DxGraphic& graphic) noexcept
{
	if (ImGui::Begin("Cube"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &matConst.specularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &matConst.specularPower, 0.0f, 100.0f);
		bool checkState = matConst.normalEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		matConst.normalEnabled = checkState ? TRUE : FALSE;

		if (changed0 || changed1 || changed2)
		{
			GetRenderObject<Graphic::PixelConstantBuffer<PSMaterialConstant>>()->Update(graphic, matConst);
		}
	}
	ImGui::End();
}