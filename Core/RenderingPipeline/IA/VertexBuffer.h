#pragma once
#include "../Bindable.h"

#include "Core/Exception/GraphicsException.h"

class VertexBuffer : public Bindable
{
public:
	template<class BufferType>
	VertexBuffer(DxGraphic& graphic, const vector<BufferType>& vertices) : stride(sizeof(BufferType))
	{
		CREATEINFOMANAGER(graphic);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.ByteWidth = UINT(sizeof(BufferType) * vertices.size());
        vertexBufferDesc.StructureByteStride = sizeof(BufferType);

        D3D11_SUBRESOURCE_DATA initData = { };
        initData.pSysMem = vertices.data();

        GRAPHIC_THROW_INFO(GetDevice(graphic)->CreateBuffer(&vertexBufferDesc, &initData, &vertexBuffer));
	}

    // Bindable을(를) 통해 상속됨
    void PipeLineSet(DxGraphic& graphic) noexcept override;

protected:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
};

