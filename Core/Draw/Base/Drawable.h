#pragma once

#include "../../DxGraphic.h"

#include "Core/Camera/CameraViewFrustumCulling.h"
#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderingManager/Technique/Technique.h"

#include <memory>
#include <DirectXMath.h>
#include <sstream>

// 전방 선언
class TechniqueBase;
class Material;
struct aiMesh;

namespace RenderGraphNameSpace { class RenderGraph; }

namespace Graphic
{
    class IndexBuffer;
    class VertexBuffer;
    class PrimitiveTopology;
    class InputLayout;
}

// 렌더링 가능한 3D 객체의 추상 기본 클래스
class Drawable
{
    template<class T>
    friend class DrawableBase;

public:
    Drawable() = default;
    Drawable(const Material& material, const aiMesh& mesh, float scale = 1.0f) noexcept;    // 머티리얼과 메시 데이터로 Drawable 객체 생성
    Drawable(const Drawable&) = delete;                                                     // 복사 생성자 삭제 (리소스 관리 안전성)

    void Submit(size_t channelFilter) const noexcept;   // 지정된 채널 필터로 렌더링 제출
    void Accept(TechniqueBase& tech);                   // 기법 객체 방문 허용 (Visitor 패턴)
    void SetRenderPipeline() const NOEXCEPTRELEASE;     // 렌더링 파이프라인 설정 (버텍스/인덱스 버퍼, 토폴로지)
    UINT GetIndexCount() const NOEXCEPTRELEASE;         // 인덱스 개수 반환

    void AddTechnique(Technique technique) noexcept;                        // 렌더링 기법 추가
    void LinkTechniques(RenderGraphNameSpace::RenderGraph&);                // 모든 기법을 렌더 그래프에 연결
    
    void SetTechniqueActive(const std::string& name, bool active) noexcept; // 특정 기법의 활성화 상태 설정
    bool GetTechniqueActive(const std::string& name) const noexcept;        // 특정 기법의 활성화 상태 반환

    void CalculateBoundingSphere(const aiMesh& mesh, float scale) noexcept; // 메시로부터 바운딩 스피어 계산
    DirectX::XMFLOAT3 GetBoundingSphereCenter() const noexcept;             // 월드 공간에서의 바운딩 스피어 중심점 반환
    float GetBoundingSphereRadius() const noexcept;                         // 월드 공간에서의 바운딩 스피어 반지름 반환

    // 뷰 절두체 컬링 체크 (객체가 카메라 시야에 있는지 확인)
    bool IsInViewFrustum(const CameraViewFrustumCulling& viewFrustum) const noexcept;

    // 객체의 변환 행렬을 반환하는 순수 가상 함수
    virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;
    virtual ~Drawable();

protected:
    std::vector<Technique> techniques;      // 이 객체에 적용될 렌더링 기법들

    // 렌더링에 필요한 기본 파이프라인 객체들
    std::shared_ptr<Graphic::IndexBuffer> indexBuffer;              // 인덱스 버퍼
    std::shared_ptr<Graphic::VertexBuffer> vertexBuffer;            // 버텍스 버퍼
    std::shared_ptr<Graphic::PrimitiveTopology> primitiveTopology;  // 프리미티브 토폴로지

    // 바운딩 스피어 정보 (로컬 공간 기준)
    DirectX::XMFLOAT3 m_boundingSphereCenter = { 0.0f, 0.0f, 0.0f }; // 바운딩 스피어 중심점
    float m_boundingSphereRadius = 1.0f;                             // 바운딩 스피어 반지름
};

