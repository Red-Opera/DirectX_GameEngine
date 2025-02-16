#include "stdafx.h"
#include "RenderPass.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Utility/StringConverter.h"

#include <sstream>

namespace RenderGraphNameSpace
{
	RenderPass::RenderPass(std::string name) noexcept : name(std::move(name))
	{

	}

	void RenderPass::Reset() NOEXCEPTRELEASE
	{

	}

	const std::vector<std::unique_ptr<PipelineDataConsumer>>& RenderPass::GetSinks() const
	{
		return dataConsumers;
	}

	PipelineDataProvider& RenderPass::GetSource(const std::string& registeredName) const
	{
		for (auto& output : dataProviders)
		{
			if (output->GetName() == registeredName)
				return *output;
		}

		std::ostringstream output;
		output << "Output 이름 [" << registeredName << "]를 " << GetName() << "에서 발견하지 못했음";

		throw RENDER_GRAPHIC_EXCEPTION(output.str());
	}

	PipelineDataConsumer& RenderPass::GetSink(const std::string& registeredName) const
	{
		for (auto& input : dataConsumers)
		{
			if (input->GetRegisteredName() == registeredName)
				return *input;
		}

		std::ostringstream output;
		output << "Input 이름 [" << registeredName << "]를 " << GetName() << "에서 발견하지 못했음";

		throw RENDER_GRAPHIC_EXCEPTION(output.str());
	}

	void RenderPass::SetSinkLinkage(const std::string& registeredName, const std::string& target)
	{
		auto& input = GetSink(registeredName);
		auto targetSplit = StringConverter::SplitString(target, ".");

		if (targetSplit.size() != 2u)
			RENDER_GRAPHIC_EXCEPTION("Input Target의 형식이 일치하지 않음");

		input.SetTarget(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}

	const std::string& RenderPass::GetName() const noexcept
	{
		return name;
	}

	void RenderPass::Finalize()
	{
		for (auto& sink : dataConsumers)
			sink->CheckLinkage();

		for (auto& output : dataProviders)
			output->CheckLinkage();
	}

	RenderPass::~RenderPass()
	{

	}

	void RenderPass::AddDataConsumer(std::unique_ptr<PipelineDataConsumer> inputIn)
	{
		for (auto& input : dataConsumers)
		{
			if (input->GetRegisteredName() == inputIn->GetRegisteredName())
				RENDER_GRAPHIC_EXCEPTION("이미 " + inputIn->GetRegisteredName() + "라는 이름의 Input이 추가되어 있음");
		}

		dataConsumers.push_back(std::move(inputIn));
	}

	void RenderPass::AddDataProvider(std::unique_ptr<PipelineDataProvider> outputIn)
	{
		for (auto& source : dataProviders)
		{
			if (source->GetName() == outputIn->GetName())
				RENDER_GRAPHIC_EXCEPTION("이미 " + outputIn->GetName() + "라는 이름의 Output이 추가되어 있음");
		}

		dataProviders.push_back(std::move(outputIn));
	}
}