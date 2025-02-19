#include "stdafx.h"
#include "Test.h"

#include "Core/Window.h"
#include "Core/DxGraphic.h"

#include "Core/Draw/Base/Image/Image.h"
#include "Core/Draw/Base/Material.h"
#include "Core/Draw/Mesh.h"
#include "Core/RenderingPipeline/Vertex.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/TypeCache.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Utility/MathInfo.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <algorithm>
#include <array>
#include <cstring>

void Test()
{
	using namespace std::string_literals;

	// data roundtrip tests
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Struct>("butts"s);
		s["butts"s].add<DynamicConstantBuffer::float3>("pubes"s);
		s["butts"s].add<DynamicConstantBuffer::float1>("dank"s);
		s.add<DynamicConstantBuffer::float1>("woot"s);
		s.add<DynamicConstantBuffer::Array>("arr"s);
		s["arr"s].set<DynamicConstantBuffer::Struct>(4);
		s["arr"s].T().add<DynamicConstantBuffer::float3>("twerk"s);
		s["arr"s].T().add<DynamicConstantBuffer::Array>("werk"s);
		s["arr"s].T()["werk"s].set<DynamicConstantBuffer::float1>(6);
		s["arr"s].T().add<DynamicConstantBuffer::Array>("meta"s);
		s["arr"s].T()["meta"s].set<DynamicConstantBuffer::Array>(6);
		s["arr"s].T()["meta"s].T().set<DynamicConstantBuffer::Matrix>(4);
		s["arr"s].T().add<DynamicConstantBuffer::Bool>("booler");

		// fails: duplicate symbol name
		// s.add<DynamicConstantBuffer::Bool>( "arr"s );
		// fails: bad symbol name
		//s.add<DynamicConstantBuffer::Bool>( "69man" );

		auto b = DynamicConstantBuffer::Buffer(std::move(s));
		const auto sig = b.GetLayout().GetSignature();

		{
			auto exp = 42.0f;
			b["woot"s] = exp;
			float act = b["woot"s];
			assert(act == exp);
		}
		{
			auto exp = 420.0f;
			b["butts"s]["dank"s] = exp;
			float act = b["butts"s]["dank"s];
			assert(act == exp);
		}
		{
			auto exp = 111.0f;
			b["arr"s][2]["werk"s][5] = exp;
			float act = b["arr"s][2]["werk"s][5];
			assert(act == exp);
		}
		{
			auto exp = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
			b["butts"s]["pubes"s] = exp;
			DirectX::XMFLOAT3 act = b["butts"s]["pubes"s];
			assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT3)));
		}
		{
			DirectX::XMFLOAT4X4 exp;
			DirectX::XMStoreFloat4x4(
				&exp,
				DirectX::XMMatrixIdentity()
			);
			b["arr"s][2]["meta"s][5][3] = exp;
			DirectX::XMFLOAT4X4 act = b["arr"s][2]["meta"s][5][3];
			assert(!std::memcmp(&exp, &act, sizeof(DirectX::XMFLOAT4X4)));
		}
		{
			auto exp = true;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert(act == exp);
		}
		{
			auto exp = false;
			b["arr"s][2]["booler"s] = exp;
			bool act = b["arr"s][2]["booler"s];
			assert(act == exp);;
		}
		// exists
		{
			assert(b["butts"s]["pubes"s].IsExist());
			assert(!b["butts"s]["fubar"s].IsExist());

			if (auto ref = b["butts"s]["pubes"s]; ref.IsExist())
			{
				DirectX::XMFLOAT3 f = ref;
				assert(f.x == 69.0f);
			}
		}
		// set if exists
		{
			assert(b["butts"s]["pubes"s].SetValue(DirectX::XMFLOAT3{ 1.0f,2.0f,3.0f }));
			auto& f3 = static_cast<const DirectX::XMFLOAT3&>(b["butts"s]["pubes"s]);
			assert(f3.x == 1.0f && f3.y == 2.0f && f3.z == 3.0f);
			assert(!b["butts"s]["phubar"s].SetValue(DirectX::XMFLOAT3{ 2.0f,2.0f,7.0f }));
		}
		const auto& cb = b;
		{
			DirectX::XMFLOAT4X4 act = cb["arr"s][2]["meta"s][5][3];
			assert(act._11 == 1.0f);
		}
	}
	// size test array of arrays
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Array>("arr");
		s["arr"].set<DynamicConstantBuffer::Array>(6);
		s["arr"].T().set<DynamicConstantBuffer::Matrix>(4);
		auto b = DynamicConstantBuffer::Buffer(std::move(s));
		auto act = b.size();
		assert(act == 16u * 4u * 4u * 6u);
	}
	// size test array of floats
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Array>("arr");
		s["arr"].set<DynamicConstantBuffer::float1>(16);
		auto b = DynamicConstantBuffer::Buffer(std::move(s));
		auto act = b.size();
		assert(act == 256u);
	}
	// size test array of structs with padding
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Array>("arr");
		s["arr"].set<DynamicConstantBuffer::Struct>(6);
		s["arr"s].T().add<DynamicConstantBuffer::float2>("a");
		s["arr"].T().add<DynamicConstantBuffer::float3>("b"s);
		auto b = DynamicConstantBuffer::Buffer(std::move(s));
		auto act = b.size();
		assert(act == 16u * 2u * 6u);
	}
	// size test array of primitive that needs padding
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Array>("arr");
		s["arr"].set<DynamicConstantBuffer::float3>(6);
		auto b = DynamicConstantBuffer::Buffer(std::move(s));
		auto act = b.size();
		assert(act == 16u * 6u);
	}
	// testing CookedLayout
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Array>("arr");
		s["arr"].set<DynamicConstantBuffer::float3>(6);
		auto cooked = DynamicConstantBuffer::TypeCache::Get(std::move(s));
		// raw is cleared after donating
		s.add<DynamicConstantBuffer::float2>("arr");
		// fails to compile, cooked returns const&
		// cooked["arr"].add<DynamicConstantBuffer::float>("buttman");
		auto b1 = DynamicConstantBuffer::Buffer(cooked);
		b1["arr"][0] = DirectX::XMFLOAT3{ 69.0f,0.0f,0.0f };
		auto b2 = DynamicConstantBuffer::Buffer(cooked);
		b2["arr"][0] = DirectX::XMFLOAT3{ 420.0f,0.0f,0.0f };
		assert(static_cast<DirectX::XMFLOAT3>(b1["arr"][0]).x == 69.0f);
		assert(static_cast<DirectX::XMFLOAT3>(b2["arr"][0]).x == 420.0f);
	}
	// specific testing scenario
	{
		DynamicConstantBuffer::EditLayout pscLayout;
		pscLayout.add<DynamicConstantBuffer::float3>("materialColor");
		pscLayout.add<DynamicConstantBuffer::float3>("specularColor");
		pscLayout.add<DynamicConstantBuffer::float1>("specularWeight");
		pscLayout.add<DynamicConstantBuffer::float1>("specularGloss");
		auto cooked = DynamicConstantBuffer::TypeCache::Get(std::move(pscLayout));
		assert(cooked.GetLayoutByteSize() == 48u);
	}
	// array non-packing
	{
		DynamicConstantBuffer::EditLayout pscLayout;
		pscLayout.add<DynamicConstantBuffer::Array>("arr");
		pscLayout["arr"].set<DynamicConstantBuffer::float1>(10);
		auto cooked = DynamicConstantBuffer::TypeCache::Get(std::move(pscLayout));
		assert(cooked.GetLayoutByteSize() == 160u);
	}
	// array of struct w/ padding
	{
		DynamicConstantBuffer::EditLayout pscLayout;
		pscLayout.add<DynamicConstantBuffer::Array>("arr");
		pscLayout["arr"].set<DynamicConstantBuffer::Struct>(10);
		pscLayout["arr"].T().add<DynamicConstantBuffer::float3>("x");
		pscLayout["arr"].T().add<DynamicConstantBuffer::float2>("y");
		auto cooked = DynamicConstantBuffer::TypeCache::Get(std::move(pscLayout));
		assert(cooked.GetLayoutByteSize() == 320u);
	}
	// testing pointer stuff
	{
		DynamicConstantBuffer::EditLayout s;
		s.add<DynamicConstantBuffer::Struct>("butts"s);
		s["butts"s].add<DynamicConstantBuffer::float3>("pubes"s);
		s["butts"s].add<DynamicConstantBuffer::float1>("dank"s);

		auto b = DynamicConstantBuffer::Buffer(std::move(s));
		const auto exp = 696969.6969f;
		b["butts"s]["dank"s] = 696969.6969f;
		assert((float&)b["butts"s]["dank"s] == exp);
		assert(*(float*)&b["butts"s]["dank"s] == exp);
		const auto exp2 = 42.424242f;
		*(float*)&b["butts"s]["dank"s] = exp2;
		assert((float&)b["butts"s]["dank"s] == exp2);
	}
	// specific testing scenario (packing error)
	{
		DynamicConstantBuffer::EditLayout lay;
		lay.add<DynamicConstantBuffer::Bool>("normalMapEnabled");
		lay.add<DynamicConstantBuffer::Bool>("specularMapEnabled");
		lay.add<DynamicConstantBuffer::Bool>("hasGlossMap");
		lay.add<DynamicConstantBuffer::float1>("specularPower");
		lay.add<DynamicConstantBuffer::float3>("specularColor");
		lay.add<DynamicConstantBuffer::float1>("specularMapWeight");
		auto buf = DynamicConstantBuffer::Buffer(std::move(lay));
		assert(buf.size() == 32u);
	}
	// specific testing scenario (array packing issues gimme a tissue)
	{
		const int maxRadius = 7;
		const int nCoef = maxRadius * 2 + 1;
		DynamicConstantBuffer::EditLayout l;
		l.add<DynamicConstantBuffer::int32>("nTaps");
		l.add<DynamicConstantBuffer::Array>("coefficients");
		l["coefficients"].set<DynamicConstantBuffer::float1>(nCoef);
		DynamicConstantBuffer::Buffer buf{ std::move(l) };
		// assert proper amount of memory allocated
		assert(buf.size() == (nCoef + 1) * 4 * 4);
		// assert array empty
		{
			const char* begin = reinterpret_cast<char*>((int*)&buf["nTaps"]);
			assert(std::all_of(begin, begin + buf.size(), [](char c) {return c == 0; }));
		}
		// assert sparse float storage
		{
			for (int i = 0; i < nCoef; i++)
			{
				buf["coefficients"][i] = 6.9f;
			}
			const auto begin = reinterpret_cast<std::array<float, 4>*>((float*)&buf["coefficients"][0]);
			const auto end = begin + nCoef;
			assert(std::all_of(begin, end, [](const auto& arr)
				{
					return arr[0] == 6.9f && arr[1] == 0.0f && arr[2] == 0.0f && arr[3] == 0.0f;
				}));
		}
	}
}

void TestDynamicMeshLoading()
{
	using namespace VertexCore;

	Assimp::Importer imp;
	const auto pScene = imp.ReadFile("Model/Sample/brick_wall/brick_wall.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	auto layout = VertexLayout{}
		.AddType(VertexLayout::Position3D)
		.AddType(VertexLayout::Normal)
		.AddType(VertexLayout::Tangent)
		.AddType(VertexLayout::BiTangent)
		.AddType(VertexLayout::Texture2D);

	VertexBuffer buf{ std::move(layout),*pScene->mMeshes[0] };

	for (auto i = 0ull, end = buf.count(); i < end; i++)
	{
		const auto a = buf[i].GetValue<VertexLayout::Position3D>();
		const auto b = buf[i].GetValue<VertexLayout::Normal>();
		const auto c = buf[i].GetValue<VertexLayout::Tangent>();
		const auto d = buf[i].GetValue<VertexLayout::BiTangent>();
		const auto e = buf[i].GetValue<VertexLayout::Texture2D>();
	}
}

void TestMaterialSystemLoading(DxGraphic& graphic)
{
	std::string path = "Model/Sample/brick_wall/brick_wall.obj";
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	Material mat{ graphic,*pScene->mMaterials[1],path };
	Mesh mesh{ graphic,mat,*pScene->mMeshes[0] };
}

void TestScaleMatrixTranslation()
{
	auto tlMat = DirectX::XMMatrixTranslation(20.f, 30.f, 40.f);
	tlMat = Math::MultipleMatrixScale(tlMat, 0.1f);
	DirectX::XMFLOAT4X4 f4;
	DirectX::XMStoreFloat4x4(&f4, tlMat);
	auto etl = Vector::GetPosition(f4);
	assert(etl.x == 2.f && etl.y == 3.f && etl.z == 4.f);
}

void D3DTestScratchPad(Window& wnd)
{
	namespace dx = DirectX;
	using namespace VertexCore;

	const auto RenderWithVS = [&gfx = wnd.GetDxGraphic()](const std::string& vsName)
		{
			const auto bitop = Graphic::PrimitiveTopology::GetRender(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			const auto layout = VertexLayout{}
				.AddType(VertexLayout::Position2D)
				.AddType(VertexLayout::ColorFloat3);

			VertexBuffer vb(layout, 3);
			vb[0].GetValue<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.0f,0.5f };
			vb[0].GetValue<VertexLayout::ColorFloat3>() = dx::XMFLOAT3{ 1.0f,0.0f,0.0f };
			vb[1].GetValue<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.5f,-0.5f };
			vb[1].GetValue<VertexLayout::ColorFloat3>() = dx::XMFLOAT3{ 0.0f,1.0f,0.0f };
			vb[2].GetValue<VertexLayout::Position2D>() = dx::XMFLOAT2{ -0.5f,-0.5f };
			vb[2].GetValue<VertexLayout::ColorFloat3>() = dx::XMFLOAT3{ 0.0f,0.0f,1.0f };

			const auto bivb = Graphic::VertexBuffer::GetRender(gfx, "##?", vb);
			const std::vector<unsigned short> idx = { 0,1,2 };
			const auto biidx = Graphic::IndexBuffer::GetRender(gfx, "##?", idx);
			const auto bips = Graphic::PixelShader::GetRender(gfx, "Shader/Test_PS.cso");
			const auto bivs = Graphic::VertexShader::GetRender(gfx, "Shader/" + vsName);
			const auto bilay = Graphic::InputLayout::GetRender(gfx, layout, *bivs);
			auto rt = Graphic::ShaderInputRenderTarget{ gfx,1280,720,0 };

			biidx->SetRenderPipeline(gfx);
			bivb->SetRenderPipeline(gfx);
			bitop->SetRenderPipeline(gfx);
			bips->SetRenderPipeline(gfx);
			bivs->SetRenderPipeline(gfx);
			bilay->SetRenderPipeline(gfx);
			rt.Clear(gfx, { 0.0f,0.0f,0.0f,1.0f });
			rt.RenderAsBuffer(gfx);
			gfx.DrawIndexed(biidx->GetIndexCount());
			gfx.GetRenderTarget()->RenderAsBuffer(gfx);

			rt.ToImage(gfx).Save("Test_" + vsName + ".png");
		};

	RenderWithVS("Test2_VS.cso");
	RenderWithVS("Test1_VS.cso");
}