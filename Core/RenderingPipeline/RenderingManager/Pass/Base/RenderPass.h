#pragma once

#include "Core/Exception/GraphicsException.h"

#include <array>
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
	class PipelineDataConsumer;
	class PipelineDataProvider;

	class RenderPass
	{
	public:
		RenderPass(std::string name) noexcept;

		virtual void Execute(DxGraphic& graphic) const NOEXCEPTRELEASE = 0;
		virtual void Reset() NOEXCEPTRELEASE;

		const std::vector<std::unique_ptr<PipelineDataConsumer>>& GetSinks() const;
		
		PipelineDataProvider& GetSource(const std::string& registeredName) const;
		PipelineDataConsumer& GetSink(const std::string& registeredName) const;

		void SetSinkLinkage(const std::string& registeredName, const std::string& target);

		const std::string& GetName() const noexcept;

		virtual void Finalize();
		virtual ~RenderPass();

	protected:
		void AddDataConsumer(std::unique_ptr<PipelineDataConsumer> input);
		void AddDataProvider(std::unique_ptr<PipelineDataProvider> output);

		std::shared_ptr<Graphic::RenderTarget> renderTarget;
		std::shared_ptr<Graphic::DepthStencil> depthStencil;

	private:
		std::vector<std::unique_ptr<PipelineDataConsumer>> dataConsumers;
		std::vector<std::unique_ptr<PipelineDataProvider>> dataProviders;

		std::string name;
	};
}