#include "stdafx.h"
#include "Drawable.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include <External/Assimp/scene.h>
#include <sstream>

using namespace Graphic;

// 머티리얼과 메시 데이터로 Drawable 객체 생성
Drawable::Drawable(const Material& material, const aiMesh& mesh, float scale) noexcept
{
    // 머티리얼로부터 버텍스/인덱스 버퍼 생성
    vertexBuffer = material.CreateVertexBuffer(mesh, scale);
    indexBuffer = material.CreateIndexBuffer(mesh);
    primitiveTopology = Graphic::PrimitiveTopology::GetRender();

    // 머티리얼의 모든 기법을 이 객체에 추가
    for (auto& tech : material.GetTechnique())
        AddTechnique(std::move(tech));

    // 메시에서 바운딩 스피어 계산
    CalculateBoundingSphere(mesh, scale);
}

// 지정된 채널 필터로 렌더링 제출
void Drawable::Submit(size_t channelFilter) const noexcept
{
    // 모든 기법에 대해 렌더링 제출
    for (const auto& technique : techniques)
        technique.Submit(*this, channelFilter);
}

// 기법 객체 방문 허용 (Visitor 패턴)
void Drawable::Accept(TechniqueBase& tech)
{
    // 모든 기법이 방문자를 받아들이도록 함
    for (auto& technique : techniques)
        technique.Accept(tech);
}

// 렌더링 파이프라인 설정 (버텍스/인덱스 버퍼, 토폴로지)
void Drawable::SetRenderPipeline() const NOEXCEPTRELEASE
{
    primitiveTopology->SetRenderPipeline();
    indexBuffer->SetRenderPipeline();
    vertexBuffer->SetRenderPipeline();
}

// 인덱스 개수 반환
UINT Drawable::GetIndexCount() const NOEXCEPTRELEASE
{
    return indexBuffer->GetIndexCount();
}

// 렌더링 기법 추가
void Drawable::AddTechnique(Technique technique) noexcept
{
    // 기법에 부모 참조 초기화 후 추가
    technique.InitializeParentReferences(*this);
    techniques.push_back(std::move(technique));
}

// 모든 기법을 렌더 그래프에 연결
void Drawable::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
    // 각 기법을 렌더 그래프에 연결
    for (auto& tech : techniques)
        tech.Link(renderGraph);
}

// 특정 기법의 활성화 상태 설정
void Drawable::SetTechniqueActive(const std::string& name, bool active) noexcept
{
    // 지정된 이름의 기법 찾아서 활성화 상태 설정
    for (auto& technique : techniques) 
    {
        if (technique.GetName() == name) 
        {
            technique.SetActive(active);
            return; // 찾았으므로 종료
        }
    }
}

// 특정 기법의 활성화 상태 반환
bool Drawable::GetTechniqueActive(const std::string& name) const noexcept
{
    // 지정된 이름의 기법 찾아서 활성화 상태 반환
    for (const auto& technique : techniques) 
    {
        if (technique.GetName() == name)
            return technique.GetAcive();
    }

    // 해당 기법을 찾지 못한 경우 false 반환
    return false;
}

// 메시로부터 바운딩 스피어 계산
void Drawable::CalculateBoundingSphere(const aiMesh& mesh, float scale) noexcept
{
#undef max
#undef min
    // 메시에 정점이 없는 경우 기본 바운딩 스피어 사용
    if (mesh.mNumVertices == 0)
    {
        m_boundingSphereCenter = { 0.0f, 0.0f, 0.0f };
        m_boundingSphereRadius = 1.0f;
        return;
    }

    // 메시의 AABB(Axis-Aligned Bounding Box) 계산을 위한 초기값 설정
    XMFLOAT3 minPoint = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    XMFLOAT3 maxPoint = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

    // 모든 정점을 검사하여 최소/최대 좌표 찾기
    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
        // 스케일 적용된 정점 좌표
        float x = mesh.mVertices[i].x * scale;
        float y = mesh.mVertices[i].y * scale;
        float z = mesh.mVertices[i].z * scale;

        // 최소값 업데이트
        minPoint.x = std::min(minPoint.x, x);
        minPoint.y = std::min(minPoint.y, y);
        minPoint.z = std::min(minPoint.z, z);

        // 최대값 업데이트
        maxPoint.x = std::max(maxPoint.x, x);
        maxPoint.y = std::max(maxPoint.y, y);
        maxPoint.z = std::max(maxPoint.z, z);
    }

    // AABB의 중심점 계산 (최소점과 최대점의 중점)
    m_boundingSphereCenter.x = (minPoint.x + maxPoint.x) * 0.5f;
    m_boundingSphereCenter.y = (minPoint.y + maxPoint.y) * 0.5f;
    m_boundingSphereCenter.z = (minPoint.z + maxPoint.z) * 0.5f;

    // 바운딩 스피어 반지름 계산 (AABB 대각선의 절반 길이)
    XMVECTOR minVec = XMLoadFloat3(&minPoint);
    XMVECTOR maxVec = XMLoadFloat3(&maxPoint);
    XMVECTOR diagonal = XMVectorSubtract(maxVec, minVec);

    // 반지름 = 대각선 길이의 절반
    m_boundingSphereRadius = Vector::GetLength(diagonal) * 0.5f;
}

// 월드 공간에서의 바운딩 스피어 중심점 반환
DirectX::XMFLOAT3 Drawable::GetBoundingSphereCenter() const noexcept
{
    // 로컬 공간의 중심점을 월드 공간으로 변환
    XMVECTOR centerVec = XMLoadFloat3(&m_boundingSphereCenter);
    XMVECTOR transformedCenter = XMVector3Transform(centerVec, GetTransformMatrix());

    XMFLOAT3 worldCenter;
    XMStoreFloat3(&worldCenter, transformedCenter);

    return worldCenter;
}

// 월드 공간에서의 바운딩 스피어 반지름 반환
float Drawable::GetBoundingSphereRadius() const noexcept
{
    // 변환 행렬에서 최대 스케일 요소 추출
    XMMATRIX transformMatrix = GetTransformMatrix();

    // 각 축의 스케일 계산 (변환 행렬의 각 행 벡터의 길이)
    float scaleX = Vector::GetLength(transformMatrix.r[0]);
    float scaleY = Vector::GetLength(transformMatrix.r[1]);
    float scaleZ = Vector::GetLength(transformMatrix.r[2]);
    
    // 가장 큰 스케일 찾기 (비균등 스케일링 대응)
    float maxScale = std::max(scaleX, std::max(scaleY, scaleZ));

    // 원래 반지름에 최대 스케일 적용
    return m_boundingSphereRadius * maxScale;
}

// 뷰 절두체 컬링 체크 (객체가 카메라 시야에 있는지 확인)
bool Drawable::IsInViewFrustum(const CameraViewFrustumCulling& viewFrustum) const noexcept
{
    // 월드 공간에서의 바운딩 스피어 중심과 반지름 구하기
    DirectX::XMFLOAT3 worldCenter = GetBoundingSphereCenter();
    float worldRadius = GetBoundingSphereRadius();

    // 카메라 절두체 내에 바운딩 스피어가 있는지 확인
    return viewFrustum.CheckSphere(worldCenter, worldRadius);
}

Drawable::~Drawable()
{

}