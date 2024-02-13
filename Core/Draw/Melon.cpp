#include "stdafx.h"
#include "Melon.h"
#include "Sphere.h"

#include "../RenderingPipeline/RenderingPipeline.h"
using namespace std;

Melon::Melon(DxGraphic& graphic, mt19937& random, uniform_real_distribution<float>& adist, uniform_real_distribution<float>& ddist, uniform_real_distribution<float>& odist, uniform_real_distribution<float>& rdist, uniform_int_distribution<int>& longdist, uniform_int_distribution<int>& latdist)
	: r(rdist(random)),
	deltaRoll(ddist(random)), deltaPitch(ddist(random)), deltaYaw(ddist(random)),
	deltaPhi(odist(random)), deltaTheta(odist(random)), deltaChi(odist(random)),
	chi(adist(random)), theta(adist(random)), phi(adist(random))
{
	if (!IsStaticInitialized())
	{
		auto pvs = make_unique<VertexShader>(graphic, L"Shader/ColorShader.hlsl");
		auto pvsbc = pvs->GetShaderCode();

		AddStaticBind(move(pvs));

		AddStaticBind(make_unique<PixelShader>(graphic, L"Shader/ColorShader.hlsl"));

		struct PixelShaderConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};
		const PixelShaderConstants cb2 =
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind(make_unique<PixelConstantBuffer<PixelShaderConstants>>(graphic, cb2));

		const vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(make_unique<InputLayout>(graphic, ied, pvsbc));

		AddStaticBind(make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};

	auto model = Sphere::MakeTesselated<Vertex>(latdist(random), longdist(random));
	
	// deform vertices of model by linear transformation
	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 1.2f));

	AddBind(make_unique<VertexBuffer>(graphic, model.vertices));

	AddIndexBuffer(make_unique<IndexBuffer>(graphic, model.indices));

	AddBind(make_unique<TransformConstantBuffer>(graphic, *this));
}

void Melon::Update(float dt) noexcept
{
	roll += deltaRoll * dt;
	pitch += deltaPitch * dt;
	yaw += deltaYaw * dt;
	theta += deltaTheta * dt;
	phi += deltaPhi * dt;
	chi += deltaChi * dt;
}

DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
