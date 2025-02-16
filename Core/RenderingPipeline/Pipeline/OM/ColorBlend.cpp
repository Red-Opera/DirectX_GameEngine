#include "stdafx.h"
#include "ColorBlend.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/RenderManager.h"

namespace Graphic
{
	ColorBlend::ColorBlend(DxGraphic& graphic, bool blending, std::optional<float> transparencyInput) : blending(blending)
	{
		CREATEINFOMANAGER(graphic);

		if (transparencyInput)
		{
			transparency.emplace();
			transparency->fill(*transparencyInput);
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& blendRenderTarget = blendDesc.RenderTarget[0];	// ���� Ÿ���� ���� ����

		// ������ ����� ���
		if (blending)
		{
			blendRenderTarget.BlendEnable = TRUE;
			//blendRenderTarget.BlendOp = D3D11_BLEND_OP_ADD;							// ������ �κ� ���� ��� ���
			//blendRenderTarget.SrcBlendAlpha = D3D11_BLEND_ZERO;						// ���� ���� ���� ���� ��
			//blendRenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;						// �Ǵ�� ���� ���� ���� ��
			//blendRenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;						// ������ �κ� ���� ���� ��� ���
			//blendRenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	// ����� ������ ����

			// ������ ������ ���
			if (transparencyInput)
			{
				blendRenderTarget.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				blendRenderTarget.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}

			else
			{
				blendRenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;			// ���� ������ ���� ����
				blendRenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// �Ǵ�� ���� ����
			}
		}

		hr = GetDevice(graphic)->CreateBlendState(&blendDesc, &blendState);

		GRAPHIC_THROW_INFO(hr);
	}

	void ColorBlend::SetTransparency(float transparencyInput) NOEXCEPTRELEASE
	{
		assert(transparency);

		return transparency->fill(transparencyInput);
	}

	float ColorBlend::GetTransparency() const NOEXCEPTRELEASE
	{
		assert(transparency);

		return transparency->front();
	}

	std::shared_ptr<ColorBlend> ColorBlend::GetRender(DxGraphic& graphic, bool blending, std::optional<float> transparency)
	{
		return RenderManager::GetRender<ColorBlend>(graphic, blending, transparency);
	}

	std::string ColorBlend::CreateID(bool blending, std::optional<float> transparency)
	{
		using namespace std::literals;

		return typeid(ColorBlend).name() + "#"s + (blending ? "b"s : "n"s) + (transparency ? "#f"s + std::to_string(*transparency) : "");
	}

	std::string ColorBlend::GetID() const noexcept
	{
		return CreateID(blending, transparency ? transparency->front() : std::optional<float>{ });
	}

	void ColorBlend::SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE
	{
		CREATEINFOMANAGER(graphic);

		const float* transparencyDatas = transparency ? transparency->data() : nullptr;

		// ���� ���¸� ������ ������ ���ο� �Է�
		GRAPHIC_THROW_INFO_ONLY(GetDeviceContext(graphic)->OMSetBlendState(blendState.Get(), transparencyDatas, 0xffffffffu));
	}
}