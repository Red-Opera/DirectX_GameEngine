#pragma once

#include "Core/Exception/WindowException.h"

class Drawable;
class DxGraphic;
class RenderStep;

namespace RenderGraphNameSpace
{
	class RenderJob
	{
	public:
		RenderJob(const RenderStep* renderStep, const Drawable* drawable);

		void Excute() const NOEXCEPTRELEASE;

	private:
		const class Drawable* drawable;
		const class RenderStep* renderStep;
	};
}