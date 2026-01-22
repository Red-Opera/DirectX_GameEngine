#pragma once

#include <DirectXMath.h>

class Matrix
{
public:
	static bool Equal(const DirectX::XMMATRIX& a, const DirectX::XMMATRIX& b, float epsilon = 0.0001f);
	static bool Equal(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b, float epsilon = 0.0001f);
};

