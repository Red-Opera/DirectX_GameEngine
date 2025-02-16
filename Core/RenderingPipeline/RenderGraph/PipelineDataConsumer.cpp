#include "stdafx.h"
#include "PipelineDataConsumer.h"
#include "PipelineDataProvider.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"
#include "Utility/MathInfo.h"

#include <sstream>
#include <cctype>

namespace RenderGraphNameSpace
{
	PipelineDataConsumer::PipelineDataConsumer(std::string registeredNameIn)
		: registeredName(std::move(registeredNameIn))
	{

		if (registeredName.empty())
			throw RENDER_GRAPHIC_EXCEPTION("Empty output name");

		const bool isNameValid = std::all_of(registeredName.begin(), registeredName.end(), [](char c) { return std::isalnum(c) || c == '_'; });

		if (!isNameValid || std::isdigit(registeredName.front()))
			throw RENDER_GRAPHIC_EXCEPTION("Invalid output name: " + registeredName);
	}

	const std::string& PipelineDataConsumer::GetRegisteredName() const noexcept
	{
		return registeredName;
	}

	const std::string& PipelineDataConsumer::GetPassName() const noexcept
	{
		return passName;
	}

	const std::string& PipelineDataConsumer::GetOutputName() const noexcept
	{
		return outputName;
	}

	void PipelineDataConsumer::SetTarget(std::string passName, std::string outputName)
	{
		{
			if (passName.empty())
				throw RENDER_GRAPHIC_EXCEPTION("Empty output name");

			const bool isNameValid = std::all_of(passName.begin(), passName.end(), [](char c) { return std::isalnum(c) || c == '_'; });

			if (passName != "$" && (!isNameValid || std::isdigit(passName.front())))
				throw RENDER_GRAPHIC_EXCEPTION("Invalid output name: " + registeredName);

			this->passName = passName;
		}

		{
			if (outputName.empty())
				throw RENDER_GRAPHIC_EXCEPTION("Empty output name");

			const bool isNameValid = std::all_of(outputName.begin(), outputName.end(), [](char c) { return std::isalnum(c) || c == '_'; });

			if (!isNameValid || std::isdigit(outputName.front()))
				throw RENDER_GRAPHIC_EXCEPTION("Invalid output name: " + registeredName);

			this->outputName = outputName;
		}
	}
}