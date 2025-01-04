#include "stdafx.h"
#include "Vector.h"

const XMVECTOR Vector::forwardV = XMLoadFloat3(&Vector::forward);
const XMVECTOR Vector::backV = XMLoadFloat3(&Vector::back);
const XMVECTOR Vector::leftV = XMLoadFloat3(&Vector::left);
const XMVECTOR Vector::rightV = XMLoadFloat3(&Vector::right);
const XMVECTOR Vector::upV = XMLoadFloat3(&Vector::up);
const XMVECTOR Vector::downV = XMLoadFloat3(&Vector::down);
const XMVECTOR Vector::allDirV = XMVectorReplicate(1.0f);

XMFLOAT3 operator+(const XMFLOAT3& lhs, float value)
{
	return XMFLOAT3{ lhs.x + value, lhs.y + value, lhs.z + value };
}

XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMFLOAT3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

XMFLOAT3 operator-(const XMFLOAT3& lhs, float value)
{
	return XMFLOAT3{ lhs.x - value, lhs.y - value, lhs.z - value };
}

XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMFLOAT3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

XMFLOAT3 operator*(const XMFLOAT3& lhs, float value)
{
	return XMFLOAT3{ lhs.x * value, lhs.y * value, lhs.z * value };
}

XMFLOAT3 operator*(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMFLOAT3{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

XMFLOAT3 operator/(const XMFLOAT3& lhs, float value)
{
	return XMFLOAT3{ lhs.x / value, lhs.y / value, lhs.z / value };
}

XMFLOAT3 operator/(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMFLOAT3{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}

Material::Color Vector::ConvertColor(DirectX::XMVECTOR vector)
{
	using namespace DirectX;

	XMVECTOR color = XMVectorAdd(vector, allDirV);
	color = color * XMVectorReplicate(255.0f / 2.0f);

	XMFLOAT3 toFloat;
	XMStoreFloat3(&toFloat, color);

	return { (UCHAR)round(toFloat.x), (UCHAR)round(toFloat.y), (UCHAR)round(toFloat.z) };
}

DirectX::XMFLOAT3 Vector::GetEulerAngle(const DirectX::XMFLOAT4X4& matrix)
{
	DirectX::XMFLOAT3 euler;

	euler.x = asinf(-matrix._32);

	if (cosf(euler.x) > 0.0001)
	{
		euler.y = atan2f(matrix._31, matrix._33);	// Yaw
		euler.z = atan2f(matrix._12, matrix._22);	// Roll
	}

	else
	{
		euler.y = 0.0f;								// Yaw
		euler.z = atan2f(-matrix._21, matrix._11);	// Roll
	}

	return euler;
}

DirectX::XMFLOAT3 Vector::GetPosition(const DirectX::XMFLOAT4X4& matrix)
{
	return { matrix._41, matrix._42, matrix._43 };
}