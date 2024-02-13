#include "stdafx.h"
#include "PixelShader.h"

#include "Core/Exception/GraphicsException.h"
using namespace std;

PixelShader::PixelShader(DxGraphic& graphic, const wstring& path)
{
	CREATEINFOMANAGER(graphic);

	DWORD shaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
    shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

    ID3D10Blob* compiledShader = 0;
    ID3D10Blob* errorMessage = 0;

    hr = D3DX11CompileFromFileW(path.c_str(), nullptr, nullptr, "PS", "ps_5_0", shaderFlags, 0, 0, &compiledShader, &errorMessage, nullptr);

    if (FAILED(hr))
        DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);

    if (errorMessage != 0)
    {
        MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
        ReleaseCOM(errorMessage);
    }
	
    GRAPHIC_THROW_INFO(GetDevice(graphic)->CreatePixelShader(
        compiledShader->GetBufferPointer(), 
        compiledShader->GetBufferSize(), 
        nullptr, 
        &pixelShader));
}

void PixelShader::PipeLineSet(DxGraphic& graphic) noexcept
{
    GetDeviceContext(graphic)->PSSetShader(pixelShader.Get(), nullptr, 0);
}