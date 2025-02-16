#pragma once
#include "PipelineDataProvider.h"

#include "Core/Exception/RenderGraphCompileException.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "Core/RenderingPipeline/Render.h"
#include "Core/RenderingPipeline/RenderTarget.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferResource.h"

#include "Utility/MathInfo.h"

#include <cctype>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace Graphic
{
	class Render;
}

namespace RenderGraphNameSpace
{
	class RenderPass;

	class PipelineDataConsumer
	{
	public:
		const std::string& GetRegisteredName() const noexcept;
		const std::string& GetPassName() const noexcept;
		const std::string& GetOutputName() const noexcept;

		void SetTarget(std::string passName, std::string outputName);
		virtual void SetConsumeData(PipelineDataProvider& source) = 0;
		virtual void CheckLinkage() const = 0;

		virtual ~PipelineDataConsumer() = default;

	protected:
		PipelineDataConsumer(std::string registeredName);

	private:
		std::string registeredName;
		std::string passName;
		std::string outputName;
	};

	template<class T>
	class DirectBufferDataConsumer : public PipelineDataConsumer
	{
		static_assert(std::is_base_of_v<Graphic::BufferResource, T>, "DirectBufferDataConsumer target type must be a BufferResource type");

	public:
		static std::unique_ptr<PipelineDataConsumer> Create(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectBufferDataConsumer>(std::move(registeredName), target);
		}

		void CheckLinkage() const override
		{
			if (!linked)
				throw RENDER_GRAPHIC_EXCEPTION("Unlinked input: " + GetRegisteredName());
		}

		void SetConsumeData(PipelineDataProvider& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.GetData());

			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
					<< " { " << typeid(T).name() << " } not compatible with { " << typeid(*source.GetData().get()).name() << " }";

				throw RENDER_GRAPHIC_EXCEPTION(oss.str());
			}

			target = std::move(p);
			linked = true;
		}

		DirectBufferDataConsumer(std::string registeredName, std::shared_ptr<T>& render)
			: PipelineDataConsumer(std::move(registeredName)), target(render)
		{

		}

	private:
		std::shared_ptr<T>& target;
		bool linked = false;
	};

	template<class T>
	class ContainRenderPipelineDataConsumer : public PipelineDataConsumer
	{
		static_assert(std::is_base_of_v<Graphic::Render, T>, "DirectBindableSink target type must be a Bindable type");

	public:
		void CheckLinkage() const override
		{
			if (!linked)
				throw RENDER_GRAPHIC_EXCEPTION("Unlinked input: " + GetRegisteredName());
		}

		void SetConsumeData(PipelineDataProvider& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.GetRender());

			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
					<< " { " << typeid(T).name() << " } does not match { " << typeid(*source.GetRender().get()).name() << " }";

				throw RENDER_GRAPHIC_EXCEPTION(oss.str());
			}

			container[index] = std::move(p);
			linked = true;
		}

		ContainRenderPipelineDataConsumer(std::string registeredName, std::vector<std::shared_ptr<Graphic::Render>>& container, size_t index)
			: PipelineDataConsumer(std::move(registeredName)), container(container), index(index)
		{

		}

	private:
		std::vector<std::shared_ptr<Graphic::Render>>& container;
		size_t index;
		bool linked = false;
	};

	template<class T>
	class DirectRenderPipelineDataConsumer : public PipelineDataConsumer
	{
		static_assert(std::is_base_of_v<Graphic::Render, T>, "DirectRenderPipelineDataConsumer target type must be a Render type");
	public:
		static std::unique_ptr<PipelineDataConsumer> Create(std::string registeredName, std::shared_ptr<T>& target)
		{
			return std::make_unique<DirectRenderPipelineDataConsumer>(std::move(registeredName), target);
		}

		void CheckLinkage() const override
		{
			if (!linked)
				throw RENDER_GRAPHIC_EXCEPTION("Unlinked input: " + GetRegisteredName());
		}

		void SetConsumeData(PipelineDataProvider& source) override
		{
			auto p = std::dynamic_pointer_cast<T>(source.GetRender());

			if (!p)
			{
				std::ostringstream oss;
				oss << "Binding input [" << GetRegisteredName() << "] to output [" << GetPassName() << "." << GetOutputName() << "] "
					<< " { " << typeid(T).name() << " } does not match { " << typeid(*source.GetRender().get()).name() << " }";

				throw RENDER_GRAPHIC_EXCEPTION(oss.str());
			}

			target = std::move(p);
			linked = true;
		}

		DirectRenderPipelineDataConsumer(std::string registeredName, std::shared_ptr<T>& target)
			: PipelineDataConsumer(std::move(registeredName)), target(target)
		{

		}

	private:
		std::shared_ptr<T>& target;
		bool linked = false;
	};
}