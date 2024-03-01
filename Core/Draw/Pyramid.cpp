#include "stdafx.h"
#include "Pyramid.h"
#include "Cone.h"

#include "../RenderingPipeline/RenderingPipeline.h"

#include <array>
using namespace std;

Pyramid::Pyramid(DxGraphic& graphic, mt19937& random, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, uniform_int_distribution<int>& tdist)
	: TestObject(graphic, random, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		auto pvs = make_unique<VertexShader>(graphic, L"Shader/BlendedLitShader.hlsl");
		auto pvsbc = pvs->GetShaderCode();
		AddStaticBind(move(pvs));
		AddStaticBind(make_unique<PixelShader>(graphic, L"Shader/BlendedLitShader.hlsl"));

		const vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		AddStaticBind(make_unique<InputLayout>(graphic, ied, pvsbc));
		AddStaticBind(make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		} colorConstant;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphic, colorConstant, 1));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		std::array<char, 4> color;
		char padding;
	};

	auto tesselation = tdist(random);
	auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);

	for (auto& vertex : model.vertices)
		vertex.color = { (char)10, (char)10, (char)255 };

	for (int i = 0; i < tesselation; i++)
		model.vertices[i * 3].color = { (char)255, (char)10, (char)10 };

	// deform mesh linearly
	model.Transform(DirectX::XMMatrixScaling(1.0f, 1.0f, 0.7f));
	model.SetNormalVector();

	AddBind(make_unique<VertexBuffer>(graphic, model.vertices));
	AddIndexBuffer(make_unique<IndexBuffer>(graphic, model.indices));

	AddBind(make_unique<TransformConstantBuffer>(graphic, *this));
}