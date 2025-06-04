#include "stdafx.h"
#include "CylinderFrame.h"

#include "Utility/MathInfo.h"

TriangleIndexList CylinderFrame::MakeTesselated(VertexCore::VertexLayout vertexLayout, int divisionCount)
{
    using namespace VertexCore;
    using VertexType = VertexLayout::VertexType;

    assert(divisionCount >= 3);

    constexpr float radius = 0.5f;
    constexpr float halfHeight = 0.5f;
    const float twoPi = Math::PI * 2;

    // [옆면] 정점 개수: top rim (0 ~ divisionCount-1) + bottom rim (divisionCount ~ 2*divisionCount-1)
    const unsigned int lateralVertexCount = 2 * static_cast<unsigned int>(divisionCount);

    // [밑면] 정점 개수: center 1개 + rim divisionCount개
    const unsigned int bottomFaceVertexCount = 1 + static_cast<unsigned int>(divisionCount);

    // [뚜껑] 정점 개수: center 1개 + rim divisionCount개
    const unsigned int topFaceVertexCount = 1 + static_cast<unsigned int>(divisionCount);

    // 총 정점 개수 = lateral + bottom + top
    const unsigned int vertexCount = lateralVertexCount + bottomFaceVertexCount + topFaceVertexCount;

    VertexBuffer vertices(std::move(vertexLayout), vertexCount);

    // [옆면] 정점 생성
    // top rim : 인덱스 0 ~ divisionCount-1
    // bottom rim : 인덱스 divisionCount ~ 2*divisionCount-1
    for (int i = 0; i < divisionCount; ++i)
    {
        float angle = twoPi * i / divisionCount;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);

        // top rim 정점 (옆면용)
        vertices[i].GetValue<VertexType::Position3D>() = { x, halfHeight, z };

        // bottom rim 정점 (옆면용)
        vertices[divisionCount + i].GetValue<VertexType::Position3D>() = { x, -halfHeight, z };
    }

    // [밑면] 정점 생성 (옆면의 bottom rim과 별개)
    int bottomCenterIndex = lateralVertexCount; // 시작 인덱스 for 밑면

    // 밑면 중심 정점
    vertices[bottomCenterIndex].GetValue<VertexType::Position3D>() = { 0.0f, -halfHeight, 0.0f };

    // 밑면 둘레 정점
    for (int i = 0; i < divisionCount; ++i)
    {
        float angle = twoPi * i / divisionCount;

        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);

        vertices[bottomCenterIndex + 1 + i].GetValue<VertexType::Position3D>() = { x, -halfHeight, z };
    }

    // [뚜껑] 정점 생성 (별도의 정점 사용하여 노멀 등 설정 가능)
    int topCenterIndex = lateralVertexCount + bottomFaceVertexCount; // 시작 인덱스 for 뚜껑

    // 뚜껑 중심 정점
    vertices[topCenterIndex].GetValue<VertexType::Position3D>() = { 0.0f, halfHeight, 0.0f };

    // 뚜껑 둘레 정점
    for (int i = 0; i < divisionCount; ++i)
    {
        float angle = twoPi * i / divisionCount;
        float x = radius * std::cos(angle);
        float z = radius * std::sin(angle);
        vertices[topCenterIndex + 1 + i].GetValue<VertexType::Position3D>() = { x, halfHeight, z };
    }

    // 인덱스 배열 생성
    std::vector<unsigned short> indices;

    // [옆면] 측면 삼각형 (각 division마다 2개의 삼각형으로 쿼드 구성)
    for (int i = 0; i < divisionCount; ++i)
    {
        int next = (i + 1) % divisionCount;

        // 삼각형 1: (top_i, bottom rim next, bottom rim i)
        indices.push_back(static_cast<unsigned short>(i));                  // top rim i
        indices.push_back(static_cast<unsigned short>(divisionCount + next)); // bottom rim next
        indices.push_back(static_cast<unsigned short>(divisionCount + i));    // bottom rim i

        // 삼각형 2: (top_i, top rim next, bottom rim next)
        indices.push_back(static_cast<unsigned short>(i));                  // top rim i
        indices.push_back(static_cast<unsigned short>(next));               // top rim next
        indices.push_back(static_cast<unsigned short>(divisionCount + next)); // bottom rim next
    }

    // [밑면] 삼각팬 구성 (winding order: center, current rim, next rim)
    for (int i = 0; i < divisionCount; ++i)
    {
        int next = (i + 1) % divisionCount;
        indices.push_back(static_cast<unsigned short>(bottomCenterIndex));                     // 밑면 중심
        indices.push_back(static_cast<unsigned short>(bottomCenterIndex + 1 + i));               // 현재 둘레 정점
        indices.push_back(static_cast<unsigned short>(bottomCenterIndex + 1 + next));            // 다음 둘레 정점
    }

    // [뚜껑] 삼각팬 구성 (winding order: center, next rim, current rim)
    for (int i = 0; i < divisionCount; ++i)
    {
        int next = (i + 1) % divisionCount;
        indices.push_back(static_cast<unsigned short>(topCenterIndex));                        // 뚜껑 중심
        indices.push_back(static_cast<unsigned short>(topCenterIndex + 1 + next));               // 다음 둘레 정점
        indices.push_back(static_cast<unsigned short>(topCenterIndex + 1 + i));                  // 현재 둘레 정점
    }

    return { std::move(vertices), std::move(indices) };
}


TriangleIndexList CylinderFrame::CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout)
{
	using VertexLayout = VertexCore::VertexLayout::VertexType;

	if (!vertexLayout)
		vertexLayout = VertexCore::VertexLayout{}.AddType(VertexLayout::Position3D);

	return MakeTesselated(std::move(*vertexLayout), 24);
}

TriangleIndexList CylinderFrame::CreateTextureFrame()
{
    using VertexType = VertexCore::VertexLayout::VertexType;

    // VertexLayout 구성: Position3D, Normal, Texture2D
    VertexCore::VertexLayout layout;
    layout.AddType(VertexType::Position3D);
    layout.AddType(VertexType::Normal);
    layout.AddType(VertexType::Texture2D);

    // 24분할로 원통 생성
    auto cylinder = MakeTesselated(std::move(layout), 24);

    const int divisionCount = 24;
    auto& vertices = cylinder.vertices;

    // [옆면] Normal 계산: 각 정점의 Position의 x, z 성분을 이용 (수평 방향)
    for (int i = 0; i < 2 * divisionCount; ++i)
    {
        auto pos = vertices[i].GetValue<VertexType::Position3D>();
        float len = std::sqrt(pos.x * pos.x + pos.z * pos.z);

        if (len > 0.0f)
            vertices[i].GetValue<VertexType::Normal>() = { pos.x / len, 0.0f, pos.z / len };

        else
            vertices[i].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, 0.0f };
    }

    // [밑면] 노멀을 (0, -1, 0)으로 수정하여 아래 방향을 가리키도록 변경
    int bottomCenterIndex = 2 * divisionCount;
    vertices[bottomCenterIndex].GetValue<VertexType::Normal>() = { 0.0f, -1.0f, 0.0f };

    for (int i = bottomCenterIndex + 1; i < bottomCenterIndex + 1 + divisionCount; ++i)
        vertices[i].GetValue<VertexType::Normal>() = { 0.0f, -1.0f, 0.0f };

    // [뚜껑] 노멀을 (0, 1, 0)으로 수정하여 위 방향을 가리키도록 변경
    int topCenterIndex = bottomCenterIndex + 1 + divisionCount;
    vertices[topCenterIndex].GetValue<VertexType::Normal>() = { 0.0f, 1.0f, 0.0f };

    for (int i = topCenterIndex + 1; i < topCenterIndex + 1 + divisionCount; ++i)
        vertices[i].GetValue<VertexType::Normal>() = { 0.0f, 1.0f, 0.0f };

    return cylinder;
}

bool CylinderFrame::GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
    std::vector<uint32_t>& indices,
    const Scale& scale)
{
    auto model = CreateFrame();
    return ExtractPhysXVertices(model, vertices, indices, scale);
}