#include "stdafx.h"
#include "RenderingPass.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"

#include <sstream>

namespace RenderGraphNameSpace
{
	// 패스 이름과 초기 렌더 객체 목록으로 생성자 초기화
	RenderingPass::RenderingPass(std::string name, std::vector<std::shared_ptr<Graphic::Render>> renders)
		: RenderPass(std::move(name)), renders(std::move(renders))
	{

	}

	// 렌더 객체를 패스에 추가
	void RenderingPass::AddRender(std::shared_ptr<Graphic::Render> render) noexcept
	{
		renders.push_back(std::move(render));
	}

	// 모든 렌더 객체의 렌더링 파이프라인을 실행
	void RenderingPass::RenderAll() const NOEXCEPTRELEASE
	{
		// 1. 렌더 타겟과 깊이 스텐실을 렌더링 파이프라인에 바인딩
		RenderBufferResources();

		// 2. 등록된 모든 렌더 객체의 렌더링 파이프라인 설정 및 실행
		for (auto& render : renders)
			render->SetRenderPipeline();
	}

	// 패스 최종화 (렌더 타겟 또는 깊이 스텐실 존재 검증)
	void RenderingPass::Finalize()
	{
		// 부모 클래스의 최종화 먼저 실행 (데이터 연결 검증)
		RenderPass::Finalize();

		// 렌더 타겟과 깊이 스텐실 중 최소 하나는 존재해야 함
		if (renderTarget == nullptr && depthStencil == nullptr)
		{
			std::ostringstream oss;
			oss << "렌더링 패스 설정 오류: 렌더링 패스 [" << GetName() << "]에 렌더 타겟 또는 깊이 스텐실 버퍼가 설정되지 않았습니다.\n"
				<< "렌더링 패스가 정상적으로 작동하려면 다음 중 최소 하나는 설정되어야 합니다:\n"
				<< "  - 렌더 타겟 (RenderTarget): 색상 데이터를 출력할 버퍼\n"
				<< "  - 깊이 스텐실 (DepthStencil): 깊이 및 스텐실 테스트를 위한 버퍼\n"
				<< "적절한 버퍼 리소스를 데이터 소비자로 추가하거나 연결해주세요.";
			throw RENDER_GRAPHIC_EXCEPTION(oss.str());
		}
	}

	// 렌더 타겟과 깊이 스텐실을 렌더링 파이프라인에 바인딩
	void RenderingPass::RenderBufferResources() const NOEXCEPTRELEASE
	{
		// 렌더 타겟이 있는 경우 : 렌더 타겟을 주 버퍼로, 깊이 스텐실을 보조 버퍼로 설정
		if (renderTarget != nullptr)
			renderTarget->RenderAsBuffer(depthStencil.get());

		// 렌더 타겟이 없고 깊이 스텐실만 있는 경우: 깊이 전용 렌더링
		else
			depthStencil->RenderAsBuffer();
	}
}