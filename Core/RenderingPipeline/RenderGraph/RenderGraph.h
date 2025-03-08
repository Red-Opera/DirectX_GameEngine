#pragma once

#include "Core/Exception/WindowException.h"

#include <string>
#include <vector>
#include <memory>

class DxGraphic;

namespace Graphic
{
	class RenderTarget;
	class DepthStencil;
}

namespace RenderGraphNameSpace
{
	class PipelineDataProvider;
	class PipelineDataConsumer;
	class RenderPass;
	class RenderQueuePass;

	class RenderGraph
	{
	public:
		RenderGraph();
		~RenderGraph();

		void Execute() NOEXCEPTRELEASE;
		void Reset() noexcept;

		RenderQueuePass& GetRenderQueue(const std::string& passName);

	protected:
		void SetSinkTarget(const std::string& sinkName, const std::string& target);
		void AddRenderPass(std::unique_ptr<RenderPass> renderPass);
		void AddGlobalProvider(std::unique_ptr<PipelineDataProvider>);
		void AddGlobalConsumer(std::unique_ptr<PipelineDataConsumer>);

		RenderPass& FindRenderPass(const std::string& name);

		void Finalize();

		std::shared_ptr<Graphic::RenderTarget> backBufferTarget;
		std::shared_ptr<Graphic::DepthStencil> masterDepth;

	private:
		void LinkSinks(RenderPass& pass);
		void LinkGlobalSinks();

		std::vector<std::unique_ptr<RenderPass>> renderPasses;
		std::vector<std::unique_ptr<PipelineDataProvider>> globalDataProviders;
		std::vector<std::unique_ptr<PipelineDataConsumer>> globalDataConsumers;

		bool isFinalized = false;
	};
}