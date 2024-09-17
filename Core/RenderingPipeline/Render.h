#pragma once

#include "RenderManager.h"

#include "../DxGraphic.h"

namespace Graphic
{
	class Render
	{
	public:
		virtual void PipeLineSet(DxGraphic& graphic) noexcept = 0;
		virtual ~Render() = default;

		virtual std::string GetID() const noexcept
		{
			assert(false);
			return "";
		}

	protected:
		static ID3D11Device* GetDevice(DxGraphic& graphic) noexcept;
		static ID3D11DeviceContext* GetDeviceContext(DxGraphic& graphic) noexcept;
		static ExceptionInfo& GetInfoManager(DxGraphic& graphic);
	};
}