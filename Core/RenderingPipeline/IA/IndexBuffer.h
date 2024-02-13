#pragma once
#include "../Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(DxGraphic& graphic, const std::vector<unsigned short>& indices);

	// Bindable을(를) 통해 상속됨
	void PipeLineSet(DxGraphic& graphic) noexcept override;
	UINT GetIndexCount() const noexcept;

protected:
	UINT indexCount;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};

