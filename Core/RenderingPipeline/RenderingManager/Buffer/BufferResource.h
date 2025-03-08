#pragma once

#include "Core/Exception/GraphicsException.h"

class DxGraphic;

namespace Graphic
{
	class BufferResource
	{
	public:
		virtual ~BufferResource() = default;

		virtual void RenderAsBuffer() NOEXCEPTRELEASE = 0;
		virtual void RenderAsBuffer(BufferResource*) NOEXCEPTRELEASE = 0;
		virtual void Clear() NOEXCEPTRELEASE = 0;
	};
}