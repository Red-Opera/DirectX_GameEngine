#include "stdafx.h"
#include "BlurOutlineRenderGraph.h"

// 렌더링 파이프라인의 핵심 컴포넌트들을 포함
#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferPassClear.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/CameraWireFramePass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/HorizontalBlurPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/LambertianRenderPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineDrawPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineMaskPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/ShadowMapPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/SkyboxPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/VerticalBlurPass.h"
#include "Core/RenderingPipeline/RenderGraph/BlurOutlineRenderingPass.h"
#include "Core/RenderingPipeline/RenderTarget.h"

// 수학 유틸리티와 GUI 라이브러리
#include "Utility/MathInfo.h"
#include "External/Imgui/imgui.h"

namespace RenderGraphNameSpace
{
    // 블러 아웃라인 렌더 그래프 생성자
    // 전체 렌더링 파이프라인을 구성하고 각 패스 간의 의존성을 설정
    BlurOutlineRenderGraph::BlurOutlineRenderGraph() : RenderGraph()
    {
        // 1단계: 렌더 타겟 클리어 패스 설정
        // 백버퍼를 초기화하여 이전 프레임의 데이터를 제거
        {
            auto pass = std::make_unique<BufferPassClear>("clearRenderTarget");
            pass->SetConsumerLinkage("buffer", "$.backbuffer");

            AddRenderPass(std::move(pass));
        }

        // 2단계: 깊이-스텐실 버퍼 클리어 패스 설정
        // 깊이 테스트를 위한 깊이 버퍼 초기화
        {
            auto pass = std::make_unique<BufferPassClear>("clearDepthStencil");
            pass->SetConsumerLinkage("buffer", "$.masterDepth");

            AddRenderPass(std::move(pass));
        }

        // 3단계: 그림자 맵 생성 패스
        // 라이트의 관점에서 깊이 정보를 캡처하여 그림자 효과 구현
        {
            auto pass = std::make_unique<ShadowMapPass>("ShadowMap");
            AddRenderPass(std::move(pass));
        }

        // 4단계: 람베르트 라이팅 패스
        // 기본적인 디퓨즈 라이팅과 그림자를 적용한 주 렌더링
        {
            auto pass = std::make_unique<LambertianRenderPass>("lambertian");
            pass->SetConsumerLinkage("ShadowMap", "ShadowMap.Map");           // 그림자 맵 입력
            pass->SetConsumerLinkage("renderTarget", "clearRenderTarget.buffer");  // 렌더 타겟 연결
            pass->SetConsumerLinkage("depthStencil", "clearDepthStencil.buffer");  // 깊이 버퍼 연결

            AddRenderPass(std::move(pass));
        }

        // 5단계: 스카이박스 렌더링 패스
        // 배경 환경을 렌더링하여 씬의 배경 제공
        {
            auto pass = std::make_unique<SkyboxPass>("Skybox");
            pass->SetConsumerLinkage("renderTarget", "lambertian.renderTarget");
            pass->SetConsumerLinkage("depthStencil", "lambertian.depthStencil");

            AddRenderPass(std::move(pass));
        }

        // 6단계: 아웃라인 마스크 패스
        // 아웃라인 효과를 적용할 객체들의 마스크 생성
        {
            auto pass = std::make_unique<OutlineMaskPass>("outlineMask");
            pass->SetConsumerLinkage("depthStencil", "Skybox.depthStencil");

            AddRenderPass(std::move(pass));
        }

        // 7단계: 블러 커널 상수 버퍼 설정
        // 가우시안 및 박스 블러를 위한 샘플링 계수와 방향 정보 준비
        {
            // 블러 커널 계수를 저장하는 상수 버퍼 생성
            {
                DynamicConstantBuffer::EditLayout layout;
                layout.add<DynamicConstantBuffer::int32>("length");                    // 커널 길이
                layout.add<DynamicConstantBuffer::Array>("sampleCoefficients");        // 샘플링 계수 배열
                layout["sampleCoefficients"].set<DynamicConstantBuffer::float1>(maxRadius * 2 + 1);

                DynamicConstantBuffer::Buffer buffer{ std::move(layout) };
                blurKernel = std::make_shared<Graphic::CachingPixelConstantBufferEx>(buffer, 0);

                // 기본 가우시안 커널로 초기화
                SetKernelGauss(radius, sigma);
                AddGlobalProvider(DirectRenderPipelineDataProvider<Graphic::CachingPixelConstantBufferEx>::Create("blurKernel", blurKernel));
            }

            // 블러 방향을 제어하는 상수 버퍼 생성
            {
                DynamicConstantBuffer::EditLayout layout;
                layout.add<DynamicConstantBuffer::Bool>("isHorizontal");               // 수평/수직 블러 구분

                DynamicConstantBuffer::Buffer buffer{ std::move(layout) };
                blurDirection = std::make_shared<Graphic::CachingPixelConstantBufferEx>(buffer, 1);

                AddGlobalProvider(DirectRenderPipelineDataProvider<Graphic::CachingPixelConstantBufferEx>::Create("blurDirection", blurDirection));
            }
        }

        // 8단계: 아웃라인 드로우 패스
        // 아웃라인 효과를 위한 초기 렌더링 수행
        {
            auto pass = std::make_unique<BlurOutlineRenderingPass>("outlineDraw", Window::GetDxGraphic().GetWidth(), Window::GetDxGraphic().GetHeight());
            AddRenderPass(std::move(pass));
        }

        // 9단계: 수평 블러 패스
        // 아웃라인에 수평 방향의 블러 효과 적용
        {
            auto pass = std::make_unique<HorizontalBlurPass>("horizontal", Window::GetDxGraphic().GetWidth(), Window::GetDxGraphic().GetHeight());
            pass->SetConsumerLinkage("scratchIn", "outlineDraw.scratchOut");        // 아웃라인 드로우 결과 입력
            pass->SetConsumerLinkage("kernel", "$.blurKernel");                     // 블러 커널 사용
            pass->SetConsumerLinkage("direction", "$.blurDirection");               // 방향 정보 사용

            AddRenderPass(std::move(pass));
        }

        // 10단계: 수직 블러 패스
        // 수평 블러 결과에 수직 방향의 블러를 추가하여 최종 블러 효과 완성
        {
            auto pass = std::make_unique<VerticalBlurPass>("vertical");
            pass->SetConsumerLinkage("renderTarget", "Skybox.renderTarget");        // 메인 렌더 타겟
            pass->SetConsumerLinkage("depthStencil", "outlineMask.depthStencil");   // 아웃라인 마스크의 깊이 정보
            pass->SetConsumerLinkage("scratchIn", "horizontal.scratchOut");         // 수평 블러 결과 입력
            pass->SetConsumerLinkage("kernel", "$.blurKernel");                     // 블러 커널 사용
            pass->SetConsumerLinkage("direction", "$.blurDirection");               // 방향 정보 사용

            AddRenderPass(std::move(pass));
        }

        // 11단계: 와이어프레임 렌더링 패스
        // 최종 결과에 와이어프레임 오버레이 추가
        {
            auto pass = std::make_unique<CameraWireFramePass>("wireframe");
            pass->SetConsumerLinkage("renderTarget", "vertical.renderTarget");
            pass->SetConsumerLinkage("depthStencil", "vertical.depthStencil");

            AddRenderPass(std::move(pass));
        }

        // 최종 출력을 백버퍼로 설정
        SetGlobalConsumerTarget("backbuffer", "wireframe.renderTarget");

        // 렌더 그래프 완료 및 의존성 검증
        Finalize();
    }

    // ImGui 윈도우들을 렌더링하는 메서드
    // 디버깅 및 실시간 파라미터 조정을 위한 UI 제공
    void BlurOutlineRenderGraph::RenderWindows()
    {
        RenderShadowWindow();                                               // 그림자 디버깅 윈도우
        RenderKernelWindow();                                               // 블러 커널 조정 윈도우
        dynamic_cast<SkyboxPass&>(FindRenderPass("Skybox")).RenderWidnow(); // 스카이박스 설정 윈도우
    }

    // 블러 커널 설정을 위한 ImGui 윈도우
    // 실시간으로 블러 타입, 반경, 시그마 값을 조정 가능
    void BlurOutlineRenderGraph::RenderKernelWindow()
    {
        if (ImGui::Begin("Kernel"))
        {
            bool filterChanged = false;
            
            // 필터 타입 선택 콤보박스 (가우시안/박스)
            {
                const char* items[] = { "Gauss","Box" };
                static const char* curItem = items[0];

                if (ImGui::BeginCombo("Filter Type", curItem))
                {
                    for (int n = 0; n < std::size(items); n++)
                    {
                        const bool isSelected = (curItem == items[n]);

                        if (ImGui::Selectable(items[n], isSelected))
                        {
                            filterChanged = true;
                            curItem = items[n];

                            // 선택된 필터 타입에 따라 커널 타입 설정
                            if (curItem == items[0])
                                kernelType = KernelType::Gauss;

                            else if (curItem == items[1])
                                kernelType = KernelType::Box;
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }
            }

            
            bool radChange = ImGui::SliderInt("Radius", &radius, 0, maxRadius);     // 블러 반경 조정 슬라이더
            bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);      // 가우시안 블러의 시그마 값 조정 슬라이더

            // 파라미터가 변경되었을 때 커널 재계산
            if (radChange || sigChange || filterChanged)
            {
                if (kernelType == KernelType::Gauss)
                    SetKernelGauss(radius, sigma);                                // 가우시안 커널 설정

                else if (kernelType == KernelType::Box)
                    SetKernelBox(radius);                                         // 박스 커널 설정
            }
        }

        ImGui::End();
    }

    // 그림자 디버깅을 위한 ImGui 윈도우
    // 그림자 맵을 파일로 덤프하는 기능 제공
    void BlurOutlineRenderGraph::RenderShadowWindow()
    {
        if (ImGui::Begin("Shadow"))
        {
            // 큐브 맵 덤프 버튼
            if (ImGui::Button("Dump Cube Map"))
                DumpShadowMap("Temp/Dump/");
        }

        ImGui::End();
    }

    // 메인 카메라 정보를 필요한 렌더 패스들에 전달
    void BlurOutlineRenderGraph::RenderMainCamera(Camera& camera)
    {
        // 람베르트 렌더링과 스카이박스에 메인 카메라 정보 제공
        dynamic_cast<LambertianRenderPass&>(FindRenderPass("lambertian")).RenderMainCamera(camera);
        dynamic_cast<SkyboxPass&>(FindRenderPass("Skybox")).RenderMainCamera(camera);
    }

    // 그림자 카메라 정보를 그림자 관련 패스들에 전달
    void BlurOutlineRenderGraph::RenderShadowCamera(Camera& camera)
    {
        // 그림자 맵 생성과 람베르트 렌더링에 그림자 카메라 정보 제공
        dynamic_cast<ShadowMapPass&>(FindRenderPass("ShadowMap")).RenderShadowCamera(camera);
        dynamic_cast<LambertianRenderPass&>(FindRenderPass("lambertian")).RenderShadowCamera(camera);
    }

    // 그림자 맵을 지정된 경로에 덤프하는 메서드
    void BlurOutlineRenderGraph::DumpShadowMap(const std::string& path)
    {
        dynamic_cast<ShadowMapPass&>(FindRenderPass("ShadowMap")).DumpShadowMap(path);
    }

    // 가우시안 블러 커널을 생성하고 설정하는 메서드
    // 반경과 시그마 값을 기반으로 가우시안 분포 계수를 계산
    void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) NOEXCEPTRELEASE
    {
        assert(radius <= maxRadius);                // 최대 반경 제한 검증

        auto kernel = blurKernel->GetBuffer();      // 커널 버퍼 가져오기
        const int length = radius * 2 + 1;          // 커널 전체 길이 계산 (중심 + 양쪽)
        kernel["length"] = length;                  // 커널 길이 설정

        float sum = 0.0f;                           // 정규화를 위한 합계 변수

        // 가우시안 분포 값들을 계산하고 저장
        for (int i = 0; i < length; i++)
        {
            const auto x = float(i - radius);                       // 중심으로부터의 거리
            const auto gaussian = Math::Gaussian(x, sigma);         // 가우시안 함수 값 계산

            sum += gaussian;                                        // 정규화를 위한 합계 누적
            kernel["sampleCoefficients"][i] = gaussian;             // 계수 배열에 저장
        }

        // 모든 계수를 정규화하여 합이 1이 되도록 조정
        for (int i = 0; i < length; i++)
            kernel["sampleCoefficients"][i] = (float)kernel["sampleCoefficients"][i] / sum;

        blurKernel->SetBuffer(kernel);                              // 설정된 커널을 버퍼에 업데이트
    }

    // 박스 블러 커널을 생성하고 설정하는 메서드
    // 모든 샘플에 동일한 가중치를 부여하는 단순한 평균 필터
    void BlurOutlineRenderGraph::SetKernelBox(int radius) NOEXCEPTRELEASE
    {
        assert(radius <= maxRadius);                                // 최대 반경 제한 검증

        auto kernel = blurKernel->GetBuffer();                      // 커널 버퍼 가져오기
        const int length = radius * 2 + 1;                          // 커널 전체 길이 계산
        kernel["length"] = length;                                  // 커널 길이 설정

        const float c = 1.0f / length;                              // 균등한 가중치 계산 (평균)

        // 모든 계수에 동일한 가중치 설정
        for (int i = 0; i < length; i++)
            kernel["sampleCoefficients"][i] = c;

        blurKernel->SetBuffer(kernel);                              // 설정된 커널을 버퍼에 업데이트
    }
}