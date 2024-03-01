#include "stdafx.h"
#include "DrawBox.h"

#include "../RenderingPipeline/RenderingPipeline.h"

#include "Utility/Imgui/imgui.h"

#include "Cube.h"

DrawBox::DrawBox(DxGraphic& graphic, std::mt19937& random, 
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material) : TestObject(graphic, random, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		// 위치 정보를 저장하는 구조체
		struct Position
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
		};

		auto model = Cube::MakeIndependentVertex<Position>();
		model.SetNormalVector();
		model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));

		AddStaticBind(std::make_unique<VertexBuffer>(graphic, model.vertices));

		auto vertexShader = std::make_unique<VertexShader>(graphic, L"Shader/LitShader.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		AddStaticBind(std::move(vertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphic, L"Shader/LitShader.hlsl"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphic, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(graphic, inputLayoutDesc, VSShaderCode));
		AddStaticBind(std::make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	else
		SetIndexFromStatic();

	AddBind(std::make_unique<TransformConstantBuffer>(graphic, *this));

	materialConstants.color = material;
	AddBind(std::make_unique<MaterialConstantBuffer>(graphic, materialConstants, 1));

	DirectX::XMStoreFloat3x3(&transformMatrix, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(random)));
}

DirectX::XMMATRIX DrawBox::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&transformMatrix) * TestObject::GetTransformXM();
}

bool DrawBox::CreateControlWindow(int id, DxGraphic& graphic) noexcept
{
	using namespace std::string_literals;

	bool dirty = false, open = true;
	if (ImGui::Begin(("Box "s + std::to_string(id)).c_str(), &open))
	{
		ImGui::Text("Material Properties");
		const auto cd = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
		const auto sid = ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity, 0.05f, 4.0f, "%.2f", 2);
		const auto spd = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f", 2);
		dirty = cd || sid || spd;

		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	}
	ImGui::End();

	if (dirty)
		SyncMaterial(graphic);

	return open;
}

void DrawBox::SyncMaterial(DxGraphic& graphic) noexcept(!_DEBUG)
{
	auto pConstPS = GetBindObject<MaterialConstantBuffer>();

	assert(pConstPS != nullptr);
	pConstPS->Update(graphic, materialConstants);
}
