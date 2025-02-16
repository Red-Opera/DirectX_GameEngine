#pragma once

#include <string>
#include <memory>

#include "Core/Exception/RenderGraphCompileException.h"

namespace Graphic
{
	class Render;
	class BufferResource;
}

namespace RenderGraphNameSpace
{
	class PipelineDataProvider
	{
	public:
		const std::string& GetName() const noexcept;

		virtual void CheckLinkage() const = 0;
		virtual std::shared_ptr<Graphic::Render> GetRender();
		virtual std::shared_ptr<Graphic::BufferResource> GetData();

		virtual ~PipelineDataProvider() = default;

	protected:
		PipelineDataProvider(std::string name);

	private:
		std::string name;
	};

	template<class T>
	class DirectBufferPipelineDataProvider : public PipelineDataProvider
	{
	public:
		static std::unique_ptr<DirectBufferPipelineDataProvider> Create(std::string name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectBufferPipelineDataProvider>(std::move(name), buffer);
		}

		DirectBufferPipelineDataProvider(std::string name, std::shared_ptr<T>& buffer)
			: PipelineDataProvider(std::move(name)), buffer(buffer)
		{

		}

		void CheckLinkage() const
		{

		}

		std::shared_ptr<Graphic::BufferResource> GetData() override
		{
			if (linked)
				throw RENDER_GRAPHIC_EXCEPTION("Mutable output bound twice: " + GetName());

			linked = true;
			return buffer;
		}

	private:
		std::shared_ptr<T>& buffer;
		bool linked = false;
	};

	template<class T>
	class DirectRenderPipelineDataProvider : public PipelineDataProvider
	{
	public:
		static std::unique_ptr<DirectRenderPipelineDataProvider> Create(std::string name, std::shared_ptr<T>& buffer)
		{
			return std::make_unique<DirectRenderPipelineDataProvider>(std::move(name), buffer);
		}

		DirectRenderPipelineDataProvider(std::string name, std::shared_ptr<T>& render)
			: PipelineDataProvider(std::move(name)), render(render)
		{

		}

		void CheckLinkage() const
		{

		}

		std::shared_ptr<Graphic::Render> GetRender() override
		{
			return render;
		}

	private:
		std::shared_ptr<T>& render;
	};
}