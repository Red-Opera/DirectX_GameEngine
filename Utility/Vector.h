#pragma once

#include <DirectXMath.h>

#include "Core/Draw/Base/Material.h"

using namespace DirectX;

class Vector
{
public:
	// ==============================================
	//			XMFLOAT3 : 3Â÷¿ø º¤ÅÍ
	// ==============================================

	static constexpr XMFLOAT3 forward	= XMFLOAT3( 0.0f,  0.0f,  1.0f);
	static constexpr XMFLOAT3 back		= XMFLOAT3( 0.0f,  0.0f, -1.0f);
	static constexpr XMFLOAT3 left		= XMFLOAT3(-1.0f,  0.0f,  0.0f);
	static constexpr XMFLOAT3 right		= XMFLOAT3( 1.0f,  0.0f,  0.0f);
	static constexpr XMFLOAT3 up		= XMFLOAT3( 0.0f,  1.0f,  0.0f);
	static constexpr XMFLOAT3 down		= XMFLOAT3( 0.0f, -1.0f,  0.0f);
	static constexpr XMFLOAT3 allDir	= XMFLOAT3( 1.0f,  1.0f,  1.0f);

	// ==============================================
	//			XMVECTOR : 4Â÷¿ø º¤ÅÍ
	// ==============================================

	static const XMVECTOR forwardV;
	static const XMVECTOR backV;
	static const XMVECTOR leftV;
	static const XMVECTOR rightV;
	static const XMVECTOR upV;
	static const XMVECTOR downV;
	static const XMVECTOR allDirV;

	// ==============================================
	//			XMFLOAT3, XMVECTOR to Convert
	// ==============================================

	static Material::Color ConvertColor(DirectX::XMVECTOR vector);

	// ==============================================
	//			XMFLOAT4X4 to Position, Rotate
	// ==============================================

	static DirectX::XMFLOAT3 GetEulerAngle(const DirectX::XMFLOAT4X4& matrix);
	static DirectX::XMFLOAT3 GetPosition(const DirectX::XMFLOAT4X4& matrix);
};

// ==============================================
//				XMFLOAT3 Operator
// ==============================================

XMFLOAT3 operator+(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator-(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator*(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator*(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator/(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator/(const XMFLOAT3& lhs, const XMFLOAT3& rhs);