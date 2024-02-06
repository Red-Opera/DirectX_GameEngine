#include "stdafx.h"
#include "DrawBox.h"

#include "../RenderingPipeline/RenderingPipeline.h"

#include "Cube.h"

DrawBox::DrawBox(DxGraphic& graphic, mt19937& random, 
	uniform_real_distribution<float>& adist, 
	uniform_real_distribution<float>& ddist, 
	uniform_real_distribution<float>& odist, 
	uniform_real_distribution<float>& rdist,
	uniform_real_distribution<float>& bdist) 
	: r(rdist(random)), deltaRoll(ddist(random)), deltaPitch(ddist(random)), deltaYaw(ddist(random)),
	deltaTheta(odist(random)), deltaPhi(odist(random)), deltaChi(odist(random)), chi(adist(random)), theta(adist(random)), phi(adist(random))
{
	if (!IsStaticInitialized())
	{
		// 위치 정보를 저장하는 구조체
		struct Position
		{
			DirectX::XMFLOAT3 position;
		};

		auto model = Cube::CreateIndex<Position>();
		model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));

		AddStaticBind(make_unique<VertexBuffer>(graphic, model.vertices));

		auto vertexShader = make_unique<VertexShader>(graphic, L"Shader/ColorShader.hlsl");
		auto VSShaderCode = vertexShader->GetShaderCode();
		AddStaticBind(move(vertexShader));

		AddStaticBind(make_unique<PixelShader>(graphic, L"Shader/ColorShader.hlsl"));

		AddStaticIndexBuffer(make_unique<IndexBuffer>(graphic, model.indices));

		struct ColorConstantBuffer
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} faceColor[8];
		};

		const ColorConstantBuffer colorConstantBuffer =
		{
			{
				{1.0f, 1.0f, 1.0f},
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{1.0f, 1.0f, 1.0f},
				{1.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 1.0f},
				{0.0f, 0.0f, 1.0f}
			}
		};

		AddStaticBind(make_unique<PixelConstantBuffer<ColorConstantBuffer>>(graphic, colorConstantBuffer));

		const vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
		{
			{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(make_unique<InputLayout>(graphic, inputLayoutDesc, VSShaderCode));
		AddStaticBind(make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	else
		SetIndexFromStatic();

	AddBind(make_unique<TransformConstantBuffer>(graphic, *this));

	DirectX::XMStoreFloat3x3(&transformMatrix, DirectX::XMMatrixScaling(1.0f, 1.0f, bdist(random)));
}

void DrawBox::Update(float dt) noexcept
{
	roll += deltaRoll * dt;
	pitch += deltaPitch * dt;
	yaw += deltaYaw * dt;

	theta += deltaTheta * dt;
	phi += deltaPhi * dt;
	chi += deltaChi * dt;
}

DirectX::XMMATRIX DrawBox::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&transformMatrix) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
