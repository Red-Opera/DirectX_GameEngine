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

Vector4 operator+(const Vector4& lhs, float value)
{
	return Vector4{ lhs.x + value, lhs.y + value, lhs.z + value, lhs.w + value };
}

Vector4 operator+(const Vector4& lhs, const Vector4& rhs)
{
	return Vector4{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}

Vector4 operator-(const Vector4& lhs, float value)
{
	return Vector4{ lhs.x - value, lhs.y - value, lhs.z - value, lhs.w - value };
}

Vector4 operator-(const Vector4& lhs, const Vector4& rhs)
{
	return Vector4{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
}

Vector4 operator*(const Vector4& lhs, float value)
{
	return Vector4{ lhs.x * value, lhs.y * value, lhs.z * value, lhs.w * value };
}

Vector4 operator*(const Vector4& lhs, const Vector4& rhs)
{
	return Vector4{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
}

Vector4 operator/(const Vector4& lhs, float value)
{
	return Vector4{ lhs.x / value, lhs.y / value, lhs.z / value, lhs.w / value };
}

Vector4 operator/(const Vector4& lhs, const Vector4 rhs)
{
	return Vector4{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
}

GraphicResource::Image::Color Vector::ConvertColor(DirectX::XMVECTOR vector)
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

Vector3::Vector3(const Vector2& vector) noexcept : x(vector.x), y(vector.y), z(0.0f)
{

}

Vector3::Vector3(const Vector4& vector) noexcept : x(vector.x), y(vector.y), z(vector.z)
{

}

Vector3::Vector3(Vector2&& vector) : x(vector.x), y(vector.y), z(0.0f)
{

}

Vector3::Vector3(Vector4&& vector) : x(vector.x), y(vector.y), z(vector.z)
{

}

Vector3& Vector3::operator=(const Vector2& vector) noexcept
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = 0.0f;

	return *this;
}

Vector3& Vector3::operator=(const Vector3& vector) noexcept
{
	if (this == &vector)
		return *this;

	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;

	return *this;
}

Vector3& Vector3::operator=(const Vector4& vector) noexcept
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;

	return *this;
}

Vector2::Vector2(const Vector3& vector) noexcept : x(vector.x), y(vector.y)
{

}

Vector2::Vector2(Vector3&& vector) : x(vector.x), y(vector.y)
{

}

Vector2::Vector2(const Vector4& vector) noexcept : x(vector.x), y(vector.y)
{

}

Vector2::Vector2(Vector4&& vector) : x(vector.x), y(vector.y) 
{

}

Vector2& Vector2::operator=(const Vector2& vector) noexcept

{
	if (this == &vector)
		return *this;

	this->x = vector.x;
	this->y = vector.y;

	return *this;
}

Vector2& Vector2::operator=(const Vector3& vector) noexcept
{
	this->x = vector.x;
	this->y = vector.y;

	return *this;
}

Vector2& Vector2::operator=(const Vector4& vector) noexcept
{
	this->x = vector.x;
	this->y = vector.y;

	return *this;
}

Vector4::Vector4(const Vector3& vector) noexcept : x(vector.x), y(vector.y), z(vector.z), w(0.0f) 
{

}

Vector4::Vector4(Vector3&& vector) : x(vector.x), y(vector.y), z(vector.z), w(0.0f) 
{

}

Vector4::Vector4(const Vector2& vector) noexcept : x(vector.x), y(vector.y), z(0.0f), w(0.0f) 
{

}

Vector4::Vector4(Vector2&& vector) : x(vector.x), y(vector.y), z(0.0f), w(0.0f)
{

}

Vector4& Vector4::operator=(const Vector2& vector) noexcept
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = 0.0f;
	this->w = 0.0f;

	return *this;
}

Vector4& Vector4::operator=(const Vector3& vector) noexcept
{
	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
	this->w = 0.0f;

	return *this;
}

Vector4& Vector4::operator=(const Vector4& vector) noexcept
{
	if (this == &vector)
		return *this;

	this->x = vector.x;
	this->y = vector.y;
	this->z = vector.z;
	this->w = vector.w;

	return *this;
}

Vector2 operator+(const Vector2& lhs, float value)
{
	return Vector2{ lhs.x + value, lhs.y + value };
}

Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x + rhs.x, lhs.y + rhs.y };
}

Vector2 operator-(const Vector2& lhs, float value)
{
	return Vector2{ lhs.x - value, lhs.y - value };
}

Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x - rhs.x, lhs.y - rhs.y };
}

Vector2 operator*(const Vector2& lhs, float value)
{
	return Vector2{ lhs.x * value, lhs.y * value };
}

Vector2 operator*(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x * rhs.x, lhs.y * rhs.y };
}

Vector2 operator/(const Vector2& lhs, float value)
{
	return Vector2{ lhs.x / value, lhs.y / value };
}

Vector2 operator/(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2{ lhs.x / rhs.x, lhs.y / rhs.y };
}

Vector3 operator+(const Vector3& lhs, float value)
{
	return Vector3{ lhs.x + value, lhs.y + value, lhs.z + value };
}

Vector3 operator+(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
}

Vector3 operator-(const Vector3& lhs, float value)
{
	return Vector3{ lhs.x - value, lhs.y - value, lhs.z - value };
}

Vector3 operator-(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

Vector3 operator*(const Vector3& lhs, float value)
{
	return Vector3{ lhs.x * value, lhs.y * value, lhs.z * value };
}

Vector3 operator*(const Vector3& lhs, const Vector3& rhs)
{
	return Vector3{ lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z };
}

Vector3 operator/(const Vector3& lhs, float value)
{
	return Vector3{ lhs.x / value, lhs.y / value, lhs.z / value };
}

Vector3 operator/(const Vector3& lhs, const Vector3 rhs)
{
	return Vector3{ lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z };
}
