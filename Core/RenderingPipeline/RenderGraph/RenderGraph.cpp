#include "stdafx.h"
#include "RenderGraph.h"

#include "../Pipeline/OM/DepthStencil.h"
#include "../RenderGraph/PipelineDataConsumer.h"
#include "../RenderGraph/PipelineDataProvider.h"
#include "../RenderingPipeline.h"
#include "../RenderingManager/Pass/Base/RenderPass.h"
#include "../RenderingManager/Pass/Base/RenderQueuePass.h"
#include "../RenderTarget.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Utility/StringConverter.h"

#include <sstream>

namespace RenderGraphNameSpace
{
	RenderGraph::RenderGraph()
		: backBufferTarget(Window::GetDxGraphic().GetRenderTarget()),
		masterDepth(std::make_shared<Graphic::OutputOnlyDepthStencil>())
	{
		AddGlobalProvider(DirectBufferPipelineDataProvider<Graphic::RenderTarget>::Create("backbuffer", backBufferTarget));
		AddGlobalProvider(DirectBufferPipelineDataProvider<Graphic::DepthStencil>::Create("masterDepth", masterDepth));
		AddGlobalConsumer(DirectBufferDataConsumer<Graphic::RenderTarget>::Create("backbuffer", backBufferTarget));
	}

	RenderGraph::~RenderGraph()
	{

	}

	void RenderGraph::SetSinkTarget(const std::string& sinkName, const std::string& target)
	{
		const auto finder = [&sinkName](const std::unique_ptr<PipelineDataConsumer>& passInput) { return passInput->GetRegisteredName() == sinkName; };
		const auto i = std::find_if(globalDataConsumers.begin(), globalDataConsumers.end(), finder);

		if (i == globalDataConsumers.end())
			throw RENDER_GRAPHIC_EXCEPTION(sinkName + "�̶�� Global Sink�� ã�� �� ����");

		auto targetSplit = StringConverter::SplitString(target, ".");

		if (targetSplit.size() != 2u)
			throw RENDER_GRAPHIC_EXCEPTION("Input Target�� �ùٸ��� �ʴ� ����");

		(*i)->SetTarget(targetSplit[0], targetSplit[1]);
	}

	void RenderGraph::AddRenderPass(std::unique_ptr<RenderPass> renderPass)
	{
		assert(!isFinalized);

		for (const auto& pass : renderPasses)
		{
			if (renderPass->GetName() == pass->GetName())
				throw RENDER_GRAPHIC_EXCEPTION(renderPass->GetName() + "�̶�� Pass �̸��� �̹� ������");
		}

		LinkSinks(*renderPass);

		renderPasses.push_back(std::move(renderPass));
	}

	void RenderGraph::AddGlobalConsumer(std::unique_ptr<PipelineDataConsumer> input)
	{
		globalDataConsumers.push_back(std::move(input));
	}

	RenderPass& RenderGraph::FindRenderPass(const std::string& name)
	{
		const auto i = std::find_if(renderPasses.begin(), renderPasses.end(), [&name](auto& renderPass) { return renderPass->GetName() == name; });

		if (i == renderPasses.end())
			throw std::runtime_error(name + "��� �̸��� Pass�� �������� ����");

		return **i;
	}

	void RenderGraph::AddGlobalProvider(std::unique_ptr<PipelineDataProvider> output)
	{
		globalDataProviders.push_back(std::move(output));
	}

	void RenderGraph::Execute() NOEXCEPTRELEASE
	{
		assert(isFinalized);

		for (auto& pass : renderPasses)
			pass->Execute();
	}

	void RenderGraph::Reset() noexcept
	{
		assert(isFinalized);

		for (auto& pass : renderPasses)
			pass->Reset();
	}

	void RenderGraph::Finalize()
	{
		assert(!isFinalized);

		for (const auto& pass : renderPasses)
			pass->Finalize();

		LinkGlobalSinks();
		isFinalized = true;
	}

	RenderQueuePass& RenderGraph::GetRenderQueue(const std::string& passName)
	{
		try
		{
			for (const auto& pass : renderPasses)
			{
				if (pass->GetName() == passName)
					return dynamic_cast<RenderQueuePass&>(*pass);
			}
		}

		catch (std::bad_cast&)
		{
			throw RENDER_GRAPHIC_EXCEPTION(passName + "��� �̸��� RenderQueuePass�� �н��� �������� ����");
		}

		throw RENDER_GRAPHIC_EXCEPTION(passName + "��� �̸��� RenderQueue�� �н��� �������� ����");
	}

	void RenderGraph::LinkSinks(RenderPass& pass)
	{
		for (auto& passInput : pass.GetSinks())
		{
			const auto& inputSourcePassName = passInput->GetPassName();

			if (inputSourcePassName.empty())
			{
				std::ostringstream output;
				output << pass.GetName() << "��� �̸��� Pass�� " << passInput->GetRegisteredName() << "�� �������� ����";

				throw RENDER_GRAPHIC_EXCEPTION(output.str());
			}

			if (inputSourcePassName == "$")
			{
				bool isRenderSet = false;

				for (auto& source : globalDataProviders)
				{
					if (source->GetName() == passInput->GetOutputName())
					{
						passInput->SetConsumeData(*source);
						isRenderSet = true;

						break;
					}
				}

				if (!isRenderSet)
				{
					std::ostringstream output;
					output << "[" + passInput->GetOutputName() << "] ��� �̸��� Globals�� �������� ����";

					throw RENDER_GRAPHIC_EXCEPTION(output.str());
				}
			}

			else
			{
				bool bound = false;
				for (auto& pass : renderPasses)
				{
					if (pass->GetName() == inputSourcePassName)
					{
						auto& source = pass->GetSource(passInput->GetOutputName());
						passInput->SetConsumeData(source);
						bound = true;

						break;
					}
				}

				if (!bound)
				{
					std::ostringstream oss;
					oss << "Pass Name [" << inputSourcePassName << "]�� �߰����� ����";

					throw RENDER_GRAPHIC_EXCEPTION(oss.str());
				}
			}
		}
	}

	void RenderGraph::LinkGlobalSinks()
	{
		for (auto& sink : globalDataConsumers)
		{
			const auto& inputSorucePassName = sink->GetPassName();

			for (auto& existingPass : renderPasses)
			{
				if (existingPass->GetName() == inputSorucePassName)
				{
					auto& source = existingPass->GetSource(sink->GetOutputName());
					sink->SetConsumeData(source);
					break;
				}
			}
		}
	}
}
