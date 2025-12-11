#include "stdafx.h"
#include "PostProcessFullScreenRenderPass.h"

#include "Core/RenderingPipeline/RenderingPipeline.h"
#include "Core/Exception/RenderGraphCompileException.h"

#include <sstream>

namespace RenderGraphNameSpace
{
    // 패스 이름으로 전체 화면 후처리 패스 생성
    PostProcessFullScreenRenderPass::PostProcessFullScreenRenderPass(const std::string name) NOEXCEPTRELEASE
        : RenderingPass(name)
    {
        // 입력 이름 유효성 검증
        if (name.empty())
        {
            std::ostringstream oss;
            oss << "후처리 패스 생성 오류: 패스 이름이 비어있습니다.\n"
                << "유효한 패스 이름을 제공해주세요.";

            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }

        try
        {
            // 전체 화면을 덮는 쿼드를 위한 버텍스 레이아웃 생성
            VertexCore::VertexLayout layout;
            layout.AddType(VertexCore::VertexLayout::Position2D); // 2D 위치만 사용

            // 전체 화면을 덮는 4개의 정점 생성 (NDC 좌표계 사용)
            VertexCore::VertexBuffer vertexBufferFull{ layout };
            vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ -1,  1 }); // 좌상단
            vertexBufferFull.emplace_back(DirectX::XMFLOAT2{  1,  1 }); // 우상단
            vertexBufferFull.emplace_back(DirectX::XMFLOAT2{ -1, -1 }); // 좌하단
            vertexBufferFull.emplace_back(DirectX::XMFLOAT2{  1, -1 }); // 우하단

            // 전체 화면 쿼드용 버텍스 버퍼 등록 ("$Full"은 글로벌 식별자)
            AddRender(Graphic::VertexBuffer::GetRender("$Full", std::move(vertexBufferFull)));

            // 두 개의 삼각형으로 사각형을 구성하는 인덱스 버퍼
            std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
            AddRender(Graphic::IndexBuffer::GetRender("$Full", std::move(indices)));

            // 후처리용 버텍스 셰이더 로드
            auto vertexShader = Graphic::VertexShader::GetRender("Shader/PostProcessing/ScreenBlur.hlsl");
            
            // 입력 레이아웃을 버텍스 셰이더와 연결
            AddRender(Graphic::InputLayout::GetRender(layout, *vertexShader));
            AddRender(std::move(vertexShader));
            
            // 삼각형 리스트 토폴로지 설정 (기본값)
            AddRender(Graphic::PrimitiveTopology::GetRender());
            
            // 래스터라이저 설정 (컬링 비활성화 - 전체 화면 쿼드를 위해)
            AddRender(Graphic::Rasterizer::GetRender(false));
        }

        catch (const std::exception& e)
        {
            std::ostringstream oss;
            oss << "후처리 패스 초기화 오류 : 렌더링 파이프라인 설정에 실패했습니다.\n"
                << "패스 이름 : [" << name << "]\n"
                << "오류 내용 : " << e.what() << "\n"
                << "셰이더 파일과 시스템 리소스를 확인해주세요.";
            throw RENDER_GRAPHIC_EXCEPTION(oss.str());
        }
    }

    // 전체 화면 쿼드 렌더링 실행
    void PostProcessFullScreenRenderPass::Execute() NOEXCEPTRELEASE
    {
        // 모든 렌더 객체의 파이프라인 설정 (부모 클래스 호출)
        RenderAll();

        // 전체 화면 쿼드 렌더링 (6개 인덱스 = 2개 삼각형)
        Window::GetDxGraphic().DrawIndexed(6u);
    }
}