#include "stdafx.h"
#include "PipelineDataProvider.h"

#include "Core/Exception/RenderGraphCompileException.h"

#include <algorithm>
#include <cctype>

namespace RenderGraphNameSpace
{
	PipelineDataProvider::PipelineDataProvider(std::string nameIn)
		: name(std::move(nameIn))
	{
		if (name.empty())
			throw RENDER_GRAPHIC_EXCEPTION("Empty output name");

		const bool isNameValid = std::all_of(name.begin(), name.end(), [](char c) { return std::isalnum(c) || c == '_'; });

		if (!isNameValid || std::isdigit(name.front()))
			throw RENDER_GRAPHIC_EXCEPTION("Invalid output name: " + name);
	}

	std::shared_ptr<Graphic::Render> PipelineDataProvider::GetRender()
	{
		throw RENDER_GRAPHIC_EXCEPTION("Output cannot be accessed as Render");
	}

	std::shared_ptr<Graphic::BufferResource> PipelineDataProvider::GetData()
	{
		throw RENDER_GRAPHIC_EXCEPTION("Output cannot be accessed as Render");
	}

	const std::string& PipelineDataProvider::GetName() const noexcept
	{
		return name;
	}
}