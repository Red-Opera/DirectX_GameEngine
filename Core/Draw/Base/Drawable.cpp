#include "stdafx.h"
#include "Drawable.h"

#include "Core/Draw/Base/Material.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderManager.h"
#include "Core/RenderingPipeline/RenderingPipeline.h"

#include <External/Assimp/scene.h>

using namespace Graphic;

Drawable::Drawable(const Material& material, const aiMesh& mesh, float scale) noexcept
{
	vertexBuffer = material.CreateVertexBuffer(mesh, scale);
	indexBuffer = material.CreateIndexBuffer(mesh);
	primitiveTopology = Graphic::PrimitiveTopology::GetRender();

	for (auto& tech : material.GetTechnique())
		AddTechnique(std::move(tech));

	// 메시에서 바운딩 스피어 계산
	CalculateBoundingSphere(mesh, scale);
}

void Drawable::Submit(size_t channelFilter) const noexcept
{
	for (const auto& technique : techniques)
		technique.Submit(*this, channelFilter);
}

void Drawable::Accept(TechniqueBase& tech)
{
	for (auto& technique : techniques)
		technique.Accept(tech);
}

void Drawable::SetRenderPipeline() const NOEXCEPTRELEASE
{
	primitiveTopology->SetRenderPipeline();
	indexBuffer->SetRenderPipeline();
	vertexBuffer->SetRenderPipeline();
}

UINT Drawable::GetIndexCount() const NOEXCEPTRELEASE
{
	return indexBuffer->GetIndexCount();
}

void Drawable::AddTechnique(Technique technique) noexcept
{
	technique.InitializeParentReferences(*this);
	techniques.push_back(std::move(technique));
}

void Drawable::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	for (auto& tech : techniques)
		tech.Link(renderGraph);
}

void Drawable::SetTechniqueActive(const std::string& name, bool active) noexcept
{
    for (auto& technique : techniques) 
    {
        if (technique.GetName() == name) 
        {
            technique.SetActive(active);
            break;
        }
    }
}

bool Drawable::GetTechniqueActive(const std::string& name) const noexcept
{
    for (const auto& technique : techniques) 
    {
        if (technique.GetName() == name)
            return technique.GetAcive();
    }

    return false;
}

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

    // 메시의 AABB(Axis-Aligned Bounding Box) 구하기
    XMFLOAT3 minPoint = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    XMFLOAT3 maxPoint = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

    // 모든 정점을 검사하여 최소/최대 좌표 찾기
    for (unsigned int i = 0; i < mesh.mNumVertices; i++)
    {
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

    // AABB의 중심점 계산
    m_boundingSphereCenter.x = (minPoint.x + maxPoint.x) * 0.5f;
    m_boundingSphereCenter.y = (minPoint.y + maxPoint.y) * 0.5f;
    m_boundingSphereCenter.z = (minPoint.z + maxPoint.z) * 0.5f;

    // 바운딩 스피어 반지름 계산
    // AABB 대각선의 절반 길이를 사용하는 방식
    XMVECTOR minVec = XMLoadFloat3(&minPoint);
    XMVECTOR maxVec = XMLoadFloat3(&maxPoint);
    XMVECTOR diagonal = XMVectorSubtract(maxVec, minVec);

    // 반지름 = 대각선 길이의 절반
    m_boundingSphereRadius = Vector::GetLength(diagonal) * 0.5f;
}

DirectX::XMFLOAT3 Drawable::GetBoundingSphereCenter() const noexcept
{
    // 오브젝트의 변환 행렬을 적용하여 월드 공간에서의 중심점 계산
    XMVECTOR centerVec = XMLoadFloat3(&m_boundingSphereCenter);
    XMVECTOR transformedCenter = XMVector3Transform(centerVec, GetTransformMatrix());

    XMFLOAT3 worldCenter;
    XMStoreFloat3(&worldCenter, transformedCenter);

    return worldCenter;
}

float Drawable::GetBoundingSphereRadius() const noexcept
{
    // 변환 행렬에서 최대 스케일 요소 추출
    XMMATRIX transformMatrix = GetTransformMatrix();

    // 가장 큰 스케일 찾기
    float scaleX = Vector::GetLength(transformMatrix.r[0]);
    float scaleY = Vector::GetLength(transformMatrix.r[1]);
    float scaleZ = Vector::GetLength(transformMatrix.r[2]);
    float maxScale = std::max(scaleX, std::max(scaleY, scaleZ));

    // 원래 반지름에 최대 스케일 적용
    return m_boundingSphereRadius * maxScale;
}

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