#include "stdafx.h"
#include "Pyramid.h"
#include "Cone.h"

#include "../RenderingPipeline/RenderingPipeline.h"
using namespace std;

Pyramid::Pyramid(DxGraphic& graphic, mt19937& random, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist) :
	r(rdist(random)), 
	deltaRoll(ddist(random)), deltaPitch(ddist(random)), deltaYaw(ddist(random)), 
	deltaPhi(odist(random)), deltaTheta(odist(random)), deltaChi(odist(random)),
	chi(adist(random)), theta(adist(random)), phi(adist(random))

{
	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} color;
		};
		auto model = Cone::MakeTesselated<Vertex>(4);
		// set vertex colors for mesh
		model.vertices[0].color = { 255,255,0 };
		model.vertices[1].color = { 255,255,0 };
		model.vertices[2].color = { 255,255,0 };
		model.vertices[3].color = { 255,255,0 };
		model.vertices[4].color = { 255,255,80 };
		model.vertices[5].color = { 255,10,0 };
		// deform mesh linearly
		model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));

		AddStaticBind(make_unique<VertexBuffer>(graphic, model.vertices));

		auto pvs = make_unique<VertexShader>(graphic, L"Shader/ColorBlendShader.hlsl");
		auto pvsbc = pvs->GetShaderCode();
		AddStaticBind(move(pvs));

		AddStaticBind(make_unique<PixelShader>(graphic, L"Shader/ColorBlendShader.hlsl"));

		AddStaticIndexBuffer(make_unique<IndexBuffer>(graphic, model.indices));

		const vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(make_unique<InputLayout>(graphic, ied, pvsbc));

		AddStaticBind(make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(make_unique<TransformConstantBuffer>(graphic, *this));
}

void Pyramid::Update(float dt) noexcept
{
	roll += deltaRoll * dt;
	pitch += deltaPitch * dt;
	yaw += deltaYaw * dt;
	theta += deltaTheta * dt;
	phi += deltaPhi * dt;
	chi += deltaChi * dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
