#pragma once
#include "../Render.h"

namespace Graphic
{
	class SamplerState : public Render
	{
	public:
		SamplerState(DxGraphic& graphic);

		// Bindable��(��) ���� ��ӵ�
		void PipeLineSet(DxGraphic& graphic) noexcept override;

		static std::shared_ptr<SamplerState> GetRender(DxGraphic& graphic);
		static std::string CreateID();
		std::string GetID() const noexcept override;

	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	};
}