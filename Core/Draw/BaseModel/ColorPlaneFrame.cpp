#include "stdafx.h"
#include "ColorPlaneFrame.h"

#include "Utility/MathInfo.h"

using namespace DirectX;

TriangleIndexList ColorPlaneFrame::MakeTesselated(VertexCore::VertexLayout vertexLayout)
{
    using namespace VertexCore;
    using VertexType = VertexCore::VertexLayout::VertexType;

    constexpr size_t initialCount = 4;
    constexpr size_t capacity = 8;  // 나중에 뒷면 정점을 위해 8개까지 저장할 수 있도록 함
    VertexBuffer vertexBuffer(std::move(vertexLayout), capacity);

    // 평면 꼭짓점 위치 (중심이 원점, 크기 1x1)
    const XMFLOAT3 positions[initialCount] = {
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f },
        {  0.5f,  0.5f, 0.0f },
        { -0.5f,  0.5f, 0.0f }
    };

    // 초기 4개 정점만 초기화
    for (size_t i = 0; i < initialCount; ++i)
        vertexBuffer[i].GetValue<VertexType::Position3D>() = positions[i];

    // 인덱스: 두 개의 삼각형 (0,1,2)와 (0,2,3)
    std::vector<unsigned short> indices = { 0, 1, 2, 0, 2, 3 };

    // 뒷면 정점 추가를 위해 버퍼 크기 조정
    vertexBuffer.Resize(capacity);

    // 뒷면 정점 추가
    for (size_t i = 0; i < initialCount; ++i) {
        vertexBuffer[initialCount + i].GetValue<VertexType::Position3D>() = vertexBuffer[i].GetValue<VertexType::Position3D>();
        // 필요한 경우 다른 속성도 복사
    }

    // 뒷면 인덱스 추가: 두 개의 삼각형 (4,5,6)와 (4,6,7)
    std::vector<unsigned short> backIndices = { 4, 6, 5, 4, 7, 6 };
    indices.insert(indices.end(), backIndices.begin(), backIndices.end());

    return TriangleIndexList(std::move(vertexBuffer), std::move(indices));
}

TriangleIndexList ColorPlaneFrame::CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout)
{
    using VertexLayout = VertexCore::VertexLayout::VertexType;

    if (!vertexLayout)
        vertexLayout = VertexCore::VertexLayout{}.AddType(VertexLayout::Position3D);

    return MakeTesselated(std::move(*vertexLayout));
}

TriangleIndexList ColorPlaneFrame::CreateTextureFrame()
{
    using VertexType = VertexCore::VertexLayout::VertexType;

    // VertexLayout 구성: Position3D, Normal, Texture2D
    VertexCore::VertexLayout layout;
    layout.AddType(VertexType::Position3D);
    layout.AddType(VertexType::Normal);
    layout.AddType(VertexType::Texture2D);

    // 평면 생성
    auto plane = MakeTesselated(std::move(layout));

    // 반전된 노멀 벡터 설정
    const XMFLOAT3 frontNormal = { 0.0f, 0.0f, 1.0f }; 
    const XMFLOAT3 backNormal = { 0.0f, 0.0f, -1.0f };

    // 앞면 노멀 설정
    for (size_t i = 0; i < 4; ++i)
        plane.vertices[i].GetValue<VertexType::Normal>() = frontNormal;

    // 뒷면 노멀 설정
    for (size_t i = 4; i < 8; ++i)
        plane.vertices[i].GetValue<VertexType::Normal>() = backNormal;

    return plane;
}

bool ColorPlaneFrame::GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
    std::vector<uint32_t>& indices,
    const Scale& scale)
{
    auto model = CreateFrame();
    return ExtractPhysXVertices(model, vertices, indices, scale);
}