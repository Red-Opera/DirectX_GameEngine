#pragma once

#include "Core/RenderingPipeline/Render.h"

namespace Graphic
{
	class ShadowSamplerState : public Render
	{
	public:
		ShadowSamplerState(DxGraphic& graphic);

		void SetBilinear(bool bilin);
		void SetUseHardwarePCF(bool isUseHardwarePCF);

		bool IsUseBilinear() const;
		bool IsUseHardwarePCF() const;

		// Render을(를) 통해 상속됨
		void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplers[4];

		size_t currentSampler;

	private:
		UINT GetCurrentSlot() const;

		static size_t GetShadowSamplerIndex(bool bilin, bool isUseHardwarePCF);
		static Microsoft::WRL::ComPtr<ID3D11SamplerState> CreateSampler(DxGraphic& graphic, bool bilin, bool isUseHardwarePCF);
	};
}