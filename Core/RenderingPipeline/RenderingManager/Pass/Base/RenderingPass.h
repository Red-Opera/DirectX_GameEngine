#pragma once

#include "RenderPass.h"

#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"

namespace Graphic { class Render; }

namespace RenderGraphNameSpace
{
	class RenderingPass : public RenderPass
	{
	protected:
		RenderingPass(std::string name, std::vector<std::shared_ptr<Graphic::Render>> renders = { });

		void AddRender(std::shared_ptr<Graphic::Render> render) noexcept;
		void RenderAll(DxGraphic& graphic) const NOEXCEPTRELEASE;

		void Finalize() override;

		template<class T>
		void AddRenderSink(std::string name)
		{
			const auto index = renders.size();
			renders.emplace_back();

			AddDataConsumer(std::make_unique<ContainRenderPipelineDataConsumer<T>>(std::move(name), renders, index));
		}

		std::shared_ptr<Graphic::RenderTarget> renderTarget;
		std::shared_ptr<Graphic::DepthStencil> depthStencil;

	private:
		void RenderBufferResources(DxGraphic& graphic) const NOEXCEPTRELEASE;

		std::vector<std::shared_ptr<Graphic::Render>> renders;
	};
}