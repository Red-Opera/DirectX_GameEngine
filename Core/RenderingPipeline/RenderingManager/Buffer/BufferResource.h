#pragma once

#include "Core/Exception/GraphicsException.h"

class DxGraphic;

namespace Graphic
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;

		virtual void RenderAsBuffer(DxGraphic&) NOEXCEPTRELEASE = 0;
		virtual void RenderAsBuffer(DxGraphic&, BufferResource*) NOEXCEPTRELEASE = 0;
		virtual void Clear(DxGraphic&) NOEXCEPTRELEASE = 0;
	};
}