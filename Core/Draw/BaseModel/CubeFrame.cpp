#include "stdafx.h"
#include "CubeFrame.h"

/**
 * @brief 기본 큐브 프레임을 생성합니다.
 * 
 * 8개의 버텍스와 12개의 삼각형(36개 인덱스)으로 구성된 단일 큐브를 생성합니다.
 * 버텍스들은 서로 공유되어 사용됩니다.
 * 
 * @param vertexLayout 선택적 버텍스 레이아웃. 지정하지 않으면 3D 위치만 포함
 * @return 버텍스와 인덱스로 구성된 큐브 메시 데이터
 */
TriangleIndexList CubeFrame::CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout)
{
    using namespace VertexCore;
    using VertexType = VertexCore::VertexLayout::VertexType;

    // 버텍스 레이아웃이 제공되지 않은 경우 기본 레이아웃 생성
    if (!vertexLayout)
    {
        vertexLayout = VertexCore::VertexLayout{ };
        vertexLayout->AddType(VertexType::Position3D);
    }

    // 큐브의 크기 정의 (변의 길이의 절반)
    constexpr float side = 1.0f / 2.0f;

    // 8개의 버텍스로 큐브 생성
    VertexBuffer vertices(std::move(*vertexLayout), 8u);
    vertices[0].GetValue<VertexType::Position3D>() = { -side, -side, -side }; // 전면 하단 왼쪽
    vertices[1].GetValue<VertexType::Position3D>() = { side, -side, -side };  // 전면 하단 오른쪽
    vertices[2].GetValue<VertexType::Position3D>() = { -side,  side, -side }; // 전면 상단 왼쪽
    vertices[3].GetValue<VertexType::Position3D>() = { side,  side, -side };  // 전면 상단 오른쪽
    vertices[4].GetValue<VertexType::Position3D>() = { -side, -side,  side }; // 후면 하단 왼쪽
    vertices[5].GetValue<VertexType::Position3D>() = { side, -side,  side };  // 후면 하단 오른쪽
    vertices[6].GetValue<VertexType::Position3D>() = { -side,  side,  side }; // 후면 상단 왼쪽
    vertices[7].GetValue<VertexType::Position3D>() = { side,  side,  side };  // 후면 상단 오른쪽

    // 12개의 삼각형(36개 인덱스)으로 큐브의 6개 면 구성
    return
    {
        std::move(vertices),
        {
            0,2,1, 2,3,1,  // 전면(Z-)
            1,3,5, 3,7,5,  // 우측면(X+)
            2,6,3, 3,6,7,  // 상단면(Y+)
            4,5,7, 4,7,6,  // 후면(Z+)
            0,4,2, 2,4,6,  // 좌측면(X-)
            0,1,4, 1,5,4   // 하단면(Y-)
        }
    };
}

/**
 * @brief 독립적인 버텍스를 가진 큐브 프레임을 생성합니다.
 * 
 * 각 면이 독립적인 버텍스를 가지므로 면마다 다른 속성(법선, 텍스처 등)을 적용할 수 있습니다.
 * 총 24개의 버텍스(면당 4개)와 36개의 인덱스를 사용합니다.
 * 
 * @param vertexLayout 사용할 버텍스 레이아웃
 * @return 독립 버텍스로 구성된 큐브 메시 데이터
 */
TriangleIndexList CubeFrame::CreateIndependentFrame(VertexCore::VertexLayout vertexLayout)
{
    using namespace VertexCore;
    using VertexType = VertexCore::VertexLayout::VertexType;

    // 큐브의 크기 정의 (변의 길이의 절반)
    constexpr float side = 1.0f / 2.0f;

    // 24개의 버텍스 생성 (면마다 4개의 버텍스)
    VertexBuffer vertices(std::move(vertexLayout), 24u);
    
    // 전면(Z-) 버텍스 (0-3)
    vertices[0].GetValue<VertexType::Position3D>() = { -side, -side, -side };
    vertices[1].GetValue<VertexType::Position3D>() = { side, -side, -side };
    vertices[2].GetValue<VertexType::Position3D>() = { -side,  side, -side };
    vertices[3].GetValue<VertexType::Position3D>() = { side,  side, -side };
    
    // 후면(Z+) 버텍스 (4-7)
    vertices[4].GetValue<VertexType::Position3D>() = { -side, -side,  side };
    vertices[5].GetValue<VertexType::Position3D>() = { side, -side,  side };
    vertices[6].GetValue<VertexType::Position3D>() = { -side,  side,  side };
    vertices[7].GetValue<VertexType::Position3D>() = { side,  side,  side };
    
    // 좌측면(X-) 버텍스 (8-11)
    vertices[8].GetValue<VertexType::Position3D>() = { -side, -side, -side };
    vertices[9].GetValue<VertexType::Position3D>() = { -side,  side, -side };
    vertices[10].GetValue<VertexType::Position3D>() = { -side, -side,  side };
    vertices[11].GetValue<VertexType::Position3D>() = { -side,  side,  side };
    
    // 우측면(X+) 버텍스 (12-15)
    vertices[12].GetValue<VertexType::Position3D>() = { side, -side, -side };
    vertices[13].GetValue<VertexType::Position3D>() = { side,  side, -side };
    vertices[14].GetValue<VertexType::Position3D>() = { side, -side,  side };
    vertices[15].GetValue<VertexType::Position3D>() = { side,  side,  side };
    
    // 하단면(Y-) 버텍스 (16-19)
    vertices[16].GetValue<VertexType::Position3D>() = { -side, -side, -side };
    vertices[17].GetValue<VertexType::Position3D>() = { side, -side, -side };
    vertices[18].GetValue<VertexType::Position3D>() = { -side, -side,  side };
    vertices[19].GetValue<VertexType::Position3D>() = { side, -side,  side };
    
    // 상단면(Y+) 버텍스 (20-23)
    vertices[20].GetValue<VertexType::Position3D>() = { -side,  side, -side };
    vertices[21].GetValue<VertexType::Position3D>() = { side,  side, -side };
    vertices[22].GetValue<VertexType::Position3D>() = { -side,  side,  side };
    vertices[23].GetValue<VertexType::Position3D>() = { side,  side,  side };

    // 각 면의 삼각형 인덱스 구성
    return {
        std::move(vertices),{
            0,2, 1,    2,3,1,   // 전면(Z-)
            4,5, 7,    4,7,6,   // 후면(Z+)
            8,10, 9,  10,11,9,  // 좌측면(X-)
            12,13,15, 12,15,14, // 우측면(X+)
            16,17,18, 18,17,19, // 하단면(Y-)
            20,23,21, 20,22,23  // 상단면(Y+)
        }
    };
}

/**
 * @brief 텍스처 매핑을 위한 큐브 프레임을 생성합니다.
 * 
 * 위치, 법선 벡터, 텍스처 좌표가 설정된 완전한 큐브 메시를 반환합니다.
 * 각 면은 독립적인 버텍스를 가지며, 각 버텍스는 적절한 법선 벡터와 텍스처 좌표를 가집니다.
 * 
 * @return 텍스처 매핑용 큐브 메시 데이터
 */
TriangleIndexList CubeFrame::CreateTextureFrame()
{
    using namespace VertexCore;
    using VertexType = VertexCore::VertexLayout::VertexType;

    // 위치, 법선, 텍스처 좌표를 포함한 독립 버텍스 큐브 생성
    auto textureVertices = CreateIndependentFrame(std::move(VertexLayout{}
        .AddType(VertexType::Position3D)
        .AddType(VertexType::Normal)
        .AddType(VertexType::Texture2D)
    ));

    // 각 버텍스에 텍스처 좌표 설정 (각 면마다 0,0 ~ 1,1 매핑)
    // 전면(Z-) 텍스처 좌표
    textureVertices.vertices[0].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f }; // 좌하단
    textureVertices.vertices[1].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f }; // 우하단
    textureVertices.vertices[2].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f }; // 좌상단
    textureVertices.vertices[3].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f }; // 우상단
    
    // 후면(Z+) 텍스처 좌표
    textureVertices.vertices[4].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
    textureVertices.vertices[5].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
    textureVertices.vertices[6].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
    textureVertices.vertices[7].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
    
    // 좌측면(X-) 텍스처 좌표
    textureVertices.vertices[8].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
    textureVertices.vertices[9].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
    textureVertices.vertices[10].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
    textureVertices.vertices[11].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
    
    // 우측면(X+) 텍스처 좌표
    textureVertices.vertices[12].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
    textureVertices.vertices[13].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
    textureVertices.vertices[14].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
    textureVertices.vertices[15].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
    
    // 하단면(Y-) 텍스처 좌표
    textureVertices.vertices[16].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
    textureVertices.vertices[17].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
    textureVertices.vertices[18].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
    textureVertices.vertices[19].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };
    
    // 상단면(Y+) 텍스처 좌표
    textureVertices.vertices[20].GetValue<VertexType::Texture2D>() = { 0.0f,0.0f };
    textureVertices.vertices[21].GetValue<VertexType::Texture2D>() = { 1.0f,0.0f };
    textureVertices.vertices[22].GetValue<VertexType::Texture2D>() = { 0.0f,1.0f };
    textureVertices.vertices[23].GetValue<VertexType::Texture2D>() = { 1.0f,1.0f };

    // 각 면별 법선 벡터 설정
    // 전면(Z-) 법선 벡터
    for (int i = 0; i < 4; i++)
        textureVertices.vertices[i].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, -1.0f };
    
    // 후면(Z+) 법선 벡터
    for (int i = 4; i < 8; i++)
        textureVertices.vertices[i].GetValue<VertexType::Normal>() = { 0.0f, 0.0f, 1.0f };
    
    // 좌측면(X-) 법선 벡터
    for (int i = 8; i < 12; i++)
        textureVertices.vertices[i].GetValue<VertexType::Normal>() = { -1.0f, 0.0f, 0.0f };
    
    // 우측면(X+) 법선 벡터
    for (int i = 12; i < 16; i++)
        textureVertices.vertices[i].GetValue<VertexType::Normal>() = { 1.0f, 0.0f, 0.0f };
    
    // 하단면(Y-) 법선 벡터
    for (int i = 16; i < 20; i++)
        textureVertices.vertices[i].GetValue<VertexType::Normal>() = { 0.0f, -1.0f, 0.0f };
    
    // 상단면(Y+) 법선 벡터
    for (int i = 20; i < 24; i++)
        textureVertices.vertices[i].GetValue<VertexType::Normal>() = { 0.0f, 1.0f, 0.0f };

    return textureVertices;
}

// PhysX 콜라이더용 정점 데이터 생성
bool CubeFrame::GetPhysXVertices(std::vector<physx::PxVec3>& vertices,
                               std::vector<uint32_t>& indices,
                               const Scale& scale)
{
    auto model = CreateFrame();
    return ExtractPhysXVertices(model, vertices, indices, scale);
}