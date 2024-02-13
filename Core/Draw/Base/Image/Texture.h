#pragma once
#include "Core/RenderingPipeline/Bindable.h"

class Texture : public Bindable
{
public:
	Texture(DxGraphic& graphic, const class Material& material);
	
	// Bindable��(��) ���� ��ӵ�
	void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
};

