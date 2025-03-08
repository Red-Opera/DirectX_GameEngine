#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "../../RenderGraph/PipelineDataProvider.h"
#include "../../RenderGraph/PipelineDataConsumer.h"

#include "Core/Camera/Camera.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ShadowCameraConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ShadowSamplerState.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"

#include <vector>

class DxGraphic;

namespace RenderGraphNameSpace
{
	class LambertianRenderPass : public RenderQueuePass
	{
	public:
		LambertianRenderPass(std::string name)
			: RenderQueuePass(std::move(name)), shadowConstantBuffer{ std::make_shared<Graphic::ShadowCameraConstantBuffer>() }
		{
			using namespace Graphic;

			AddRender(shadowConstantBuffer);

			AddDataConsumer(DirectBufferDataConsumer<Graphic::RenderTarget>::Create("renderTarget", renderTarget));
			AddDataConsumer(DirectBufferDataConsumer<Graphic::DepthStencil>::Create("depthStencil", depthStencil));
			AddRenderSink<Graphic::Render>("ShadowMap");
			AddRender(std::make_shared<Graphic::ShadowSamplerState>());
			AddRender(std::make_shared<Graphic::SamplerState>(Graphic::SamplerState::TextureFilter::Anisotropic, false, 2));

			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::RenderTarget>::Create("renderTarget", renderTarget));
			AddDataProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(Stencil::GetRender(Stencil::DrawMode::Off));
		}

		void Execute() const NOEXCEPTRELEASE override
		{
			assert(mainCamera != nullptr);

			shadowConstantBuffer->Update();
			mainCamera->RenderToGraphic();
			RenderQueuePass::Execute();
		}

		void RenderMainCamera(const Camera& camera) noexcept { mainCamera = &camera; }
		void RenderShadowCamera(const Camera& camera) noexcept { shadowConstantBuffer->SetCamera(&camera); }

	private:
		std::shared_ptr<Graphic::ShadowCameraConstantBuffer> shadowConstantBuffer;

		const Camera* mainCamera = nullptr;
	};
}