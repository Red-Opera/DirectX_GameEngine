#include "stdafx.h"
#include "ImportObjectTest.h"

#include "../Base/Drawable.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#pragma comment(lib, "Utility\\assimp\\assimp-vc143-mt.lib")

#include "Utility/assimp/Importer.hpp"
#include "Utility/assimp/scene.h"
#include "Utility/assimp/postprocess.h"

#include "Core/RenderingPipeline/Vertex.h"

using namespace Graphic;

ImportObjectTest::ImportObjectTest(DxGraphic& graphic, std::mt19937& random, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, DirectX::XMFLOAT3 material, float scale) : TestObject(graphic, random, adist, ddist, odist, rdist)
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	using VertexCore::VertexLayout;
	VertexCore::VertexBuffer vertexBuffer(std::move(VertexLayout{}.
		AddType(VertexLayout::Position3D).
		AddType(VertexLayout::Normal)));

	Assimp::Importer importer;
	const auto model = importer.ReadFile("Model/Monster.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	const auto mesh = model->mMeshes[0];

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		vertexBuffer.emplace_back(
			DirectX::XMFLOAT3{ mesh->mVertices[i].x * scale, mesh->mVertices[i].y * scale , mesh->mVertices[i].z * scale },
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]));
	}

	std::vector<USHORT> indices;
	indices.reserve(mesh->mNumFaces * 3);

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		const auto& face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);

	}

	const auto path = "Model/Monster";

	AddBind(VertexBuffer::GetRender(graphic, path, vertexBuffer));
	AddBind(IndexBuffer::GetRender(graphic, path,indices));

	auto vertexShader = VertexShader::GetRender(graphic, "Shader/LitShader.hlsl");
	auto VSShaderCode = vertexShader->GetShaderCode();
	AddBind(std::move(vertexShader));

	AddBind(PixelShader::GetRender(graphic, "Shader/LitShader.hlsl"));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	AddBind(InputLayout::GetRender(graphic, vertexBuffer.GetVertexLayout(), VSShaderCode));

	AddBind(PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} matConst;

	matConst.color = material;
	AddBind(PixelConstantBuffer<PSMaterialConstant>::GetRender(graphic, matConst, 1u));

	AddBind(std::make_shared<TransformConstantBuffer>(graphic, *this));
}