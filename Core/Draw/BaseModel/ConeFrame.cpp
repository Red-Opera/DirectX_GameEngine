#include "stdafx.h"
#include "ConeFrame.h"

TriangleIndexList ConeFrame::MakeTesselated(VertexCore::VertexLayout vertexLayout, int longDiv)
{
	using namespace VertexCore;
	using VertexType = VertexLayout::VertexType;

	assert(longDiv >= 3);

	const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const float longitudeAngle = 2.0f * Math::PI / longDiv;

	// base vertices: 0 ~ (longDiv - 1)
	VertexBuffer vertices(std::move(vertexLayout), longDiv + 2);

	// 1) 원주상의 정점들
	for (int iLong = 0; iLong < longDiv; iLong++)
	{
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);
		DirectX::XMStoreFloat3(
			&vertices[iLong].GetValue<VertexType::Position3D>(),
			v
		);
	}

	// 2) 원뿔 밑면 중심 (center)
	vertices[longDiv].GetValue<VertexType::Position3D>() = { 0.0f, 0.0f, -1.0f };
	const auto iCenter = (unsigned short)longDiv;   // <--- 인덱스: longDiv

	// 3) 원뿔 꼭지점 (tip)
	vertices[longDiv + 1].GetValue<VertexType::Position3D>() = { 0.0f, 0.0f, 1.0f };
	const auto iTip = (unsigned short)(longDiv + 1);   // <--- 인덱스: longDiv + 1

	// 이후 인덱스 구성
	std::vector<unsigned short> indices;

	// (a) 밑면 삼각형들
	//    - 중심(iCenter)과 iLong, (iLong+1)%longDiv로 이어지는 삼각형
	for (unsigned short iLong = 0; iLong < longDiv; iLong++)
	{
		indices.push_back(iCenter);
		indices.push_back((iLong + 1) % longDiv);
		indices.push_back(iLong);
	}

	// (b) 옆면 삼각형들
	//    - iLong, (iLong+1)%longDiv, iTip로 이어지는 삼각형
	for (unsigned short iLong = 0; iLong < longDiv; iLong++)
	{
		indices.push_back(iLong);
		indices.push_back((iLong + 1) % longDiv);
		indices.push_back(iTip);
	}

	// 최종 리턴
	return { std::move(vertices), std::move(indices) };
}

TriangleIndexList ConeFrame::MakeTesselatedSeparateBottom(VertexCore::VertexLayout vertexLayout, int longDiv)
{
	using namespace VertexCore;
	using VertexType = VertexLayout::VertexType;

	assert(longDiv >= 3);

	// 원뿔 밑면과 옆면을 나누어 생성하기 위해
	//  - 밑면 원주: 0 ~ (longDiv - 1)
	//  - 옆면 원주: longDiv ~ (2*longDiv - 1)
	//  - 밑면 중심(iCenter): 2*longDiv
	//  - 꼭지점(iTip): 2*longDiv + 1
	//
	// 총 정점 수 = 2*longDiv + 2
	VertexBuffer vertices(std::move(vertexLayout), 2 * longDiv + 2);

	// ----------------------
	// 1) 밑면 원주 (Bottom Ring) 정점: Normal = (0,0,-1)
	// ----------------------
	// pos: (cosθ, sinθ, -1) 형태(회전 변환 사용)
	const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const float dTheta = 2.0f * Math::PI / longDiv;

	for (int i = 0; i < longDiv; i++)
	{
		const float theta = dTheta * i;
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(theta)
		);
		DirectX::XMStoreFloat3(
			&vertices[i].GetValue<VertexType::Position3D>(),
			v
		);
		// 밑면용 정점 노멀은 -Z
		vertices[i].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, -1.0f };
	}

	// ----------------------
	// 2) 옆면 원주 (Side Ring) 정점: 옆면 노멀 별도 계산
	// ----------------------
	for (int i = 0; i < longDiv; i++)
	{
		const float theta = dTheta * i;
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(theta)
		);
		const int idx = longDiv + i; // 옆면 원주 위치
		DirectX::XMStoreFloat3(
			&vertices[idx].GetValue<VertexType::Position3D>(),
			v
		);
		// 일단 0으로 초기화 후, 아래에서 옆면 노멀 계산
		vertices[idx].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, 0.0f };
	}

	// ----------------------
	// 3) 밑면 중심 (iCenter) & 꼭지점 (iTip)
	// ----------------------
	const auto iCenter = static_cast<unsigned short>(2 * longDiv);
	vertices[iCenter].GetValue<VertexType::Position3D>() = { 0.0f, 0.0f, -1.0f };
	vertices[iCenter].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, -1.0f };

	const auto iTip = static_cast<unsigned short>(2 * longDiv + 1);
	vertices[iTip].GetValue<VertexType::Position3D>() = { 0.0f, 0.0f, 1.0f };
	vertices[iTip].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, 1.0f };

	// ----------------------
	// 인덱스 버퍼 구성
	// ----------------------
	std::vector<unsigned short> indices;
	indices.reserve(6 * longDiv); // 대략 삼각형 2*longDiv 개

	// (a) 밑면 삼각형
	//     - [iCenter, i, i+1] 구조
	for (unsigned short i = 0; i < longDiv; i++)
	{
		unsigned short iNext = (i + 1) % longDiv;
		indices.push_back(iCenter);
		indices.push_back(iNext);
		indices.push_back(i);
	}

	// (b) 옆면 삼각형
	//     - [i, i+1, iTip], 단 i가 "옆면 원주 인덱스"여야 하므로 offset 필요
	for (unsigned short i = 0; i < longDiv; i++)
	{
		const unsigned short iNext = (i + 1) % longDiv;
		const unsigned short i0 = longDiv + i;      // 옆면 ring
		const unsigned short i1 = longDiv + iNext;  // 옆면 ring

		indices.push_back(i0);
		indices.push_back(i1);
		indices.push_back(iTip);
	}

	// ----------------------
	// 옆면 원주 노멀 계산
	// ----------------------
	// pos = (cosθ, sinθ, -1) 형태이므로, 기존 코드처럼 s = sqrt(1^2 + 2^2) = sqrt(5)
	const float s = sqrtf(5.0f);
	for (int i = 0; i < longDiv; i++)
	{
		const int idx = longDiv + i;
		auto& pos = vertices[idx].GetValue<VertexType::Position3D>();
		auto& normal = vertices[idx].GetValue<VertexType::Normal>();

		float theta = atan2f(pos.y, pos.x); // pos가 (r*cosθ, r*sinθ, -1)이므로
		normal.x = (2.0f * cosf(theta)) / s;
		normal.y = (2.0f * sinf(theta)) / s;
		normal.z = 1.0f / s;
	}

	return { std::move(vertices), std::move(indices) };
}

TriangleIndexList ConeFrame::MakeTesselatedIndependentFaces(int longDiv)
{
	using namespace VertexCore;
	using VertexType = VertexLayout::VertexType;

	assert(longDiv >= 3);

	const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
	const float longitudeAngle = 2.0f * Math::PI / longDiv;

	VertexBuffer vertices(VertexLayout{}.AddType(VertexType::Position3D), longDiv * 4 + 1);

	// cone vertices
	const auto iCone = (unsigned short)vertices.count();

	for (int iLong = 0; iLong < longDiv; iLong++)
	{
		const float thetas[] =
		{
			longitudeAngle * iLong,
			longitudeAngle * (((iLong + 1) == longDiv) ? 0 : (iLong + 1))
		};

		vertices[iLong * 3].GetValue<VertexType::Position3D>() = { 0.0f, 0.0f, 1.0f };

		int index = 1;
		for (auto theta : thetas)
		{
			const auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(theta)
			);

			DirectX::XMStoreFloat3(&vertices[iLong * 3 + index].GetValue<VertexType::Position3D>(), v);
			index++;
		}
	}

	// base vertices
	const auto iBaseCenter = (unsigned short)(3 * longDiv);
	vertices[3 * longDiv].GetValue<VertexType::Position3D>() = { 0.0f, 0.0f, -1.0f };
	const auto iBaseEdge = iBaseCenter + 1;

	for (int iLong = 0; iLong < longDiv; iLong++)
	{
		auto v = DirectX::XMVector3Transform(
			base,
			DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
		);

		DirectX::XMStoreFloat3(&vertices[3 * longDiv + 1 + iLong].GetValue<VertexType::Position3D>(), v);
	}

	std::vector<unsigned short> indices;

	// cone indices
	for (unsigned short i = 0; i < longDiv * 3; i++)
	{
		indices.push_back(i + iCone);
	}

	// base indices
	for (unsigned short iLong = 0; iLong < longDiv; iLong++)
	{
		indices.push_back(iBaseCenter);
		indices.push_back((iLong + 1) % longDiv + iBaseEdge);
		indices.push_back(iLong + iBaseEdge);
	}

	return { std::move(vertices),std::move(indices) };
}

TriangleIndexList ConeFrame::CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout)
{
	using VertexLayout = VertexCore::VertexLayout::VertexType;

	if (!vertexLayout)
		vertexLayout = VertexCore::VertexLayout{}.AddType(VertexLayout::Position3D);

	return MakeTesselated(std::move(*vertexLayout), 24);
}

TriangleIndexList ConeFrame::CreateTextureFrameSeparateBottom()
{
	using VertexType = VertexCore::VertexLayout::VertexType;

	// 원하는 VertexLayout 구성
	VertexCore::VertexLayout layout;
	layout.AddType(VertexType::Position3D);
	layout.AddType(VertexType::Normal);
	layout.AddType(VertexType::Texture2D);

	// 24분할로 원뿔 생성 (밑면/옆면 분리)
	auto cone = MakeTesselatedSeparateBottom(std::move(layout), 24);

	return cone;
}