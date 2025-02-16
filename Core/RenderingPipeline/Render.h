#pragma once

#include "Core/DxGraphicResource.h"
#include "Core/Exception/GraphicsException.h"

#include <memory>
#include <string>

class Drawable;
class TechniqueBase;
class DxGraphic;

namespace Graphic
{
	class Render : public DxGraphicResource
	{
	public:
		virtual void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE = 0;
		virtual void InitializeParentReference(const Drawable&) noexcept { }
		virtual void Accept(TechniqueBase&) { }

		virtual ~Render() = default;

		virtual std::string GetID() const noexcept
		{
			assert(false);
			return "";
		}
	};

	class RenderInstance : public Render
	{
	public:
		virtual std::unique_ptr<RenderInstance> Instance() const noexcept = 0;
	};
}