#include "stdafx.h"
#include "Cylinder.h"
#include "Prism.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"

Cylinder::Cylinder(DxGraphic& graphic, std::mt19937& random,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	std::uniform_int_distribution<int>& tdist) : TestObject(graphic, random, adist, ddist, odist, rdist)
{
	if (!IsStaticInitialized())
	{
		auto pvs = std::make_unique<VertexShader>(graphic, L"Shader/LitShader.hlsl");
		auto pvsbc = pvs->GetShaderCode();

		AddStaticBind(std::move(pvs));
		AddStaticBind(std::make_unique<PixelShader>(graphic, L"Shader/IndexedLitShader.hlsl"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(graphic, ied, pvsbc));

		AddStaticBind(std::make_unique<PrimitiveTopology>(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3A colors[6] = {
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
				{1.0f,0.0f,1.0f},
				{0.0f,1.0f,1.0f},
			};
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
		} matConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphic, matConst, 1u));
	}
	
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	const auto instance = Prism::MakeTesselatedIndependentCapNormals<Vertex>(tdist(random));
	AddBind(std::make_unique<VertexBuffer>(graphic, instance.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(graphic, instance.indices));

	AddBind(std::make_unique<TransformConstantBuffer>(graphic, *this));
}