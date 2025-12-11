#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "Core/Camera/Camera.h"
#include "Core/Draw/BaseModel/CubeFrame.h"
#include "Core/Draw/BaseModel/SphereFrame.h"
#include "Core/Draw/BaseModel/TextureCube.h"
#include "Core/RenderingPipeline/Pipeline/IA/PrimitiveTopology.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"
#include "Core/RenderingPipeline/Pipeline/IA/InputLayout.h"
#include "Core/RenderingPipeline/Pipeline/IA/VertexBuffer.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SkyboxTransformConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/Rasterizer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"

#include <vector>
#include <sstream>

// 전방 선언
class DxGraphic;

namespace RenderGraphNameSpace
{
    // 스카이박스(배경 하늘)를 렌더링하는 패스 클래스
    class SkyboxPass : public RenderingPass
    {
    public:
        // 패스 이름으로 생성자 초기화 (모든 렌더링 파이프라인 설정)
        SkyboxPass(std::string name)
            : RenderingPass(std::move(name))
        {
            using namespace Graphic;

            // 렌더 타겟과 깊이 스텐실을 입력으로 받는 데이터 소비자 등록
            AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));
            AddDataConsumer(DirectBufferDataConsumer<DepthStencil>::Create("depthStencil", depthStencil));

            // 스카이박스 렌더링 파이프라인 구성
            AddRender(std::make_shared<TextureCube>("Images/SpaceBox"));                    // 큐브맵 텍스처
            AddRender(Stencil::GetRender(Stencil::DrawMode::DepthFirst));                   // 깊이 우선 스텐실
            AddRender(Rasterizer::GetRender(true));                                         // 래스터라이저 (컬링 활성화)
            AddRender(std::make_shared<SkyboxTransformConstantBuffer>());                   // 변환 행렬 상수 버퍼
            AddRender(PixelShader::GetRender("Shader/Skybox.hlsl"));                        // 픽셀 셰이더
            AddRender(PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)); // 삼각형 리스트 토폴로지

            // 버텍스 셰이더와 기하학적 데이터 설정
            {
                auto vertexShaderCode = VertexShader::GetRender("Shader/Skybox.hlsl");

                // 큐브 형태의 스카이박스 기하학적 데이터 생성
                {
                    auto cube = CubeFrame::CreateFrame();

                    const auto geometryTag = "$Skybox";
                    cubeVertexBuffer = VertexBuffer::GetRender(geometryTag, std::move(cube.vertices));
                    cubeIndexBuffer = IndexBuffer::GetRender(geometryTag, std::move(cube.indices));
                    cubeIndexCount = (UINT)cube.indices.size();

                    // 입력 레이아웃 설정 (버텍스 셰이더와 버텍스 데이터 연결)
                    AddRender(InputLayout::GetRender(cube.vertices.GetVertexLayout(), *vertexShaderCode));
                }

                // 구 형태의 스카이박스 기하학적 데이터 생성
                {
                    auto sphere = SphereFrame::CreateFrame();

                    const auto geometryTag = "$SkySphere";
                    sphereVertexBuffer = VertexBuffer::GetRender(geometryTag, std::move(sphere.vertices));
                    sphereIndexBuffer = IndexBuffer::GetRender(geometryTag, std::move(sphere.indices));
                    sphereIndexCount = (UINT)sphere.indices.size();
                }
                
                // 버텍스 셰이더를 렌더링 파이프라인에 추가
                AddRender(std::move(vertexShaderCode));
            }

            // 렌더 타겟과 깊이 스텐실을 출력으로 제공하는 데이터 제공자 등록
            AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
            AddDataProvider(DirectBufferPipelineDataProvider<DepthStencil>::Create("depthStencil", depthStencil));
        }

        // 스카이박스 렌더링 실행 메서드
        void Execute() NOEXCEPTRELEASE override
        {
            // 메인 카메라가 설정되지 않은 경우 예외 발생
            if (mainCamera == nullptr)
            {
                std::ostringstream oss;
                oss << "스카이박스 렌더링 오류: 스카이박스 패스 [" << GetName() << "]에 메인 카메라가 설정되지 않았습니다.\n"
                    << "RenderMainCamera() 메서드를 사용하여 유효한 카메라 객체를 설정해주세요.\n"
                    << "스카이박스는 카메라의 위치와 방향 정보가 필요합니다.";
                throw RENDER_GRAPHIC_EXCEPTION(oss.str());
            }

            // 메인 카메라의 변환 행렬을 그래픽 파이프라인에 설정
            mainCamera->RenderToGraphic();

            UINT indexCount;

            // 선택된 기하학적 형태에 따라 버퍼 설정
            if (isSphere)
            {
                // 구 형태 스카이박스 사용
                sphereVertexBuffer->SetRenderPipeline();
                sphereIndexBuffer->SetRenderPipeline();
                indexCount = sphereIndexCount;
            }

            else
            {
                // 큐브 형태 스카이박스 사용
                cubeVertexBuffer->SetRenderPipeline();
                cubeIndexBuffer->SetRenderPipeline();
                indexCount = cubeIndexCount;
            }

            // 모든 렌더 객체의 파이프라인 설정 및 실제 렌더링 수행
            RenderAll();
            Window::GetDxGraphic().DrawIndexed(indexCount);
        }

        // 스카이박스 설정을 위한 ImGui 윈도우 렌더링
        void RenderWidnow()
        {
            // 구/큐브 형태 선택 체크박스
            if (ImGui::Begin("Skybox"))
                ImGui::Checkbox("Use Sphere", &isSphere);

            ImGui::End();
        }

        // 메인 카메라 설정 메서드
        void RenderMainCamera(const Camera& camera) noexcept
        {
            mainCamera = &camera;
        }

    private:
        const Camera* mainCamera = nullptr; // 렌더링에 사용할 메인 카메라

        // 큐브 형태 스카이박스용 렌더링 리소스
        std::shared_ptr<Graphic::VertexBuffer> cubeVertexBuffer;  // 큐브 버텍스 버퍼
        std::shared_ptr<Graphic::IndexBuffer> cubeIndexBuffer;    // 큐브 인덱스 버퍼
        UINT cubeIndexCount;                                      // 큐브 인덱스 개수

        // 구 형태 스카이박스용 렌더링 리소스
        std::shared_ptr<Graphic::VertexBuffer> sphereVertexBuffer; // 구 버텍스 버퍼
        std::shared_ptr<Graphic::IndexBuffer> sphereIndexBuffer;   // 구 인덱스 버퍼
        UINT sphereIndexCount;                                     // 구 인덱스 개수

        bool isSphere = true; // 구 형태 사용 여부 (기본값: true)
    };
}