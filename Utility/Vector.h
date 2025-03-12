#pragma once

#include <DirectXMath.h>

#include "Core/Draw/Base/Image/Image.h"

using namespace DirectX;

class Vector2;
class Vector3;
class Vector4;

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

	static GraphicResource::Image::Color ConvertColor(DirectX::XMVECTOR vector);

	// ==============================================
	//			XMFLOAT4X4 to Position, Rotate
	// ==============================================

	static DirectX::XMFLOAT3 GetEulerAngle(const DirectX::XMFLOAT4X4& matrix);
	static DirectX::XMFLOAT3 GetPosition(const DirectX::XMFLOAT4X4& matrix);
};

// ==============================================
//					Vector 2D
// ==============================================
class Vector2
{
public:
	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2(const Vector2& vector) : x(vector.x), y(vector.y) {}
	Vector2(Vector2&& vector) noexcept : x(vector.x), y(vector.y) {}

	Vector2(const Vector4& vector) noexcept;
	Vector2(const Vector3& vector) noexcept;
	Vector2(Vector4&& vector);
	Vector2(Vector3&& vector);

	Vector2& operator=(const Vector2& vector) noexcept;
	Vector2& operator=(const Vector3& vector) noexcept;
	Vector2& operator=(const Vector4& vector) noexcept;

	static const Vector2 zero;
	static const Vector2 up;
	static const Vector2 down;
	static const Vector2 left;
	static const Vector2 right;
	static const Vector2 one;

	float x, y;
};
using Position2D = Vector2;

const Vector2 Vector2::zero		= Vector2( 0.0f,  0.0f);
const Vector2 Vector2::up		= Vector2( 0.0f,  1.0f);
const Vector2 Vector2::down		= Vector2( 0.0f, -1.0f);
const Vector2 Vector2::left		= Vector2(-1.0f,  0.0f);
const Vector2 Vector2::right	= Vector2( 1.0f,  0.0f);
const Vector2 Vector2::one		= Vector2( 1.0f,  1.0f);

class Vector3
{
public:
	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3(const Vector3& vector) : x(vector.x), y(vector.y), z(vector.z) {}
	Vector3(Vector3&& vector) noexcept : x(vector.x), y(vector.y), z(vector.z) {}

	Vector3(const Vector2& vector) noexcept;
	Vector3(const Vector4& vector) noexcept;
	Vector3(Vector2&& vector);
	Vector3(Vector4&& vector);

	Vector3& operator=(const Vector2& vector) noexcept;
	Vector3& operator=(const Vector3& vector) noexcept;
	Vector3& operator=(const Vector4& vector) noexcept;

	static const Vector3 zero;
	static const Vector3 forward;
	static const Vector3 back;
	static const Vector3 left;
	static const Vector3 right;
	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 one;

	float x, y, z;	
};
using Position = Vector3;
using Rotation = Vector3;
using Scale = Vector3;

const Vector3 Vector3::zero		= Vector3( 0.0f,  0.0f,  0.0f);
const Vector3 Vector3::forward	= Vector3( 0.0f,  0.0f,  1.0f);
const Vector3 Vector3::back		= Vector3( 0.0f,  0.0f, -1.0f);
const Vector3 Vector3::left		= Vector3(-1.0f,  0.0f,  0.0f);
const Vector3 Vector3::right	= Vector3( 1.0f,  0.0f,  0.0f);
const Vector3 Vector3::up		= Vector3( 0.0f,  1.0f,  0.0f);
const Vector3 Vector3::down		= Vector3( 0.0f, -1.0f,  0.0f);
const Vector3 Vector3::one		= Vector3( 1.0f,  1.0f,  1.0f);


class Vector4
{
public:
	Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4(const XMVECTOR& vector) : x(XMVectorGetX(vector)), y(XMVectorGetY(vector)), z(XMVectorGetZ(vector)), w(XMVectorGetW(vector)) {}
	Vector4(const Vector4& vector) : x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}
	Vector4(Vector4&& vector) noexcept : x(vector.x), y(vector.y), z(vector.z), w(vector.w) {}

	Vector4(const Vector3& vector) noexcept;
	Vector4(const Vector2& vector) noexcept;
	Vector4(Vector3&& vector);
	Vector4(Vector2&& vector);

	Vector4& operator=(const Vector2& vector) noexcept;
	Vector4& operator=(const Vector3& vector) noexcept;
	Vector4& operator=(const Vector4& vector) noexcept;

	static const Vector4 zero;
	static const Vector4 forward;
	static const Vector4 back;
	static const Vector4 left;
	static const Vector4 right;
	static const Vector4 up;
	static const Vector4 down;
	static const Vector4 reverseOpaque;
	static const Vector4 opaque;
	static const Vector4 one;

	float x, y, z, w;
};

const Vector4 Vector4::zero				= Vector4( 0.0f,  0.0f,  0.0f,  0.0f);
const Vector4 Vector4::forward			= Vector4( 0.0f,  0.0f,  1.0f,  0.0f);
const Vector4 Vector4::back				= Vector4( 0.0f,  0.0f, -1.0f,  0.0f);
const Vector4 Vector4::left				= Vector4(-1.0f,  0.0f,  0.0f,  0.0f);
const Vector4 Vector4::right			= Vector4( 1.0f,  0.0f,  0.0f,  0.0f);
const Vector4 Vector4::up				= Vector4( 0.0f,  1.0f,  0.0f,  0.0f);
const Vector4 Vector4::down				= Vector4( 0.0f, -1.0f,  0.0f,  0.0f);
const Vector4 Vector4::reverseOpaque	= Vector4( 0.0f,  0.0f,  0.0f, -1.0f);
const Vector4 Vector4::opaque			= Vector4( 0.0f,  0.0f,  0.0f,  1.0f);
const Vector4 Vector4::one				= Vector4( 1.0f,  1.0f,  1.0f,  1.0f);

// ==============================================
//		Vector2, Vectro3, Vector4 Operator
// ==============================================
Vector2 operator+(const Vector2& lhs, float value);
Vector2 operator+(const Vector2& lhs, const Vector2& rhs);

Vector2 operator-(const Vector2& lhs, float value);
Vector2 operator-(const Vector2& lhs, const Vector2& rhs);

Vector2 operator*(const Vector2& lhs, float value);
Vector2 operator*(const Vector2& lhs, const Vector2& rhs);

Vector2 operator/(const Vector2& lhs, float value);
Vector2 operator/(const Vector2& lhs, const Vector2& rhs);

Vector3 operator+(const Vector3& lhs, float value);
Vector3 operator+(const Vector3& lhs, const Vector3& rhs);

Vector3 operator-(const Vector3& lhs, float value);
Vector3 operator-(const Vector3& lhs, const Vector3& rhs);

Vector3 operator*(const Vector3& lhs, float value);
Vector3 operator*(const Vector3& lhs, const Vector3& rhs);

Vector3 operator/(const Vector3& lhs, float value);
Vector3 operator/(const Vector3& lhs, const Vector3 rhs);

XMFLOAT3 operator+(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator-(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator*(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator*(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

XMFLOAT3 operator/(const XMFLOAT3& lhs, float value);
XMFLOAT3 operator/(const XMFLOAT3& lhs, const XMFLOAT3& rhs);

Vector4 operator+(const Vector4& lhs, float value);
Vector4 operator+(const Vector4& lhs, const Vector4& rhs);

Vector4 operator-(const Vector4& lhs, float value);
Vector4 operator-(const Vector4& lhs, const Vector4& rhs);

Vector4 operator*(const Vector4& lhs, float value);
Vector4 operator*(const Vector4& lhs, const Vector4& rhs);

Vector4 operator/(const Vector4& lhs, float value);
Vector4 operator/(const Vector4& lhs, const Vector4 rhs);