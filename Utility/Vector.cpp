#include "stdafx.h"
#include "Vector.h"

const XMVECTOR Vector::forwardV = XMLoadFloat3(&Vector::forward);
const XMVECTOR Vector::backV = XMLoadFloat3(&Vector::back);
const XMVECTOR Vector::leftV = XMLoadFloat3(&Vector::left);
const XMVECTOR Vector::rightV = XMLoadFloat3(&Vector::right);
const XMVECTOR Vector::upV = XMLoadFloat3(&Vector::up);
const XMVECTOR Vector::downV = XMLoadFloat3(&Vector::down);

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