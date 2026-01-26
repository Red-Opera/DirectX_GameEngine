#include "stdafx.h"
#include "Vector.h"

#include <xmmintrin.h>

const XMVECTOR Vector::forwardV = XMLoadFloat3(&Vector::forward);
const XMVECTOR Vector::backV = XMLoadFloat3(&Vector::back);
const XMVECTOR Vector::leftV = XMLoadFloat3(&Vector::left);
const XMVECTOR Vector::rightV = XMLoadFloat3(&Vector::right);
const XMVECTOR Vector::upV = XMLoadFloat3(&Vector::up);
const XMVECTOR Vector::downV = XMLoadFloat3(&Vector::down);
const XMVECTOR Vector::allDirV = XMVectorReplicate(1.0f);
const XMVECTOR Vector::identityQuaternionV = XMQuaternionIdentity();

const Vector2 Vector2::zero		= Vector2( 0.0f,  0.0f);
const Vector2 Vector2::up		= Vector2( 0.0f,  1.0f);
const Vector2 Vector2::down		= Vector2( 0.0f, -1.0f);
const Vector2 Vector2::left		= Vector2(-1.0f,  0.0f);
const Vector2 Vector2::right	= Vector2( 1.0f,  0.0f);
const Vector2 Vector2::one		= Vector2( 1.0f,  1.0f);

const Vector3 Vector3::zero		= Vector3( 0.0f,  0.0f,  0.0f);
const Vector3 Vector3::forward	= Vector3( 0.0f,  0.0f,  1.0f);
const Vector3 Vector3::back		= Vector3( 0.0f,  0.0f, -1.0f);
const Vector3 Vector3::left		= Vector3(-1.0f,  0.0f,  0.0f);
const Vector3 Vector3::right	= Vector3( 1.0f,  0.0f,  0.0f);
const Vector3 Vector3::up		= Vector3( 0.0f,  1.0f,  0.0f);
const Vector3 Vector3::down		= Vector3( 0.0f, -1.0f,  0.0f);
const Vector3 Vector3::one		= Vector3( 1.0f,  1.0f,  1.0f);

const Vector4 Vector4::zero				= Vector4( 0.0f,  0.0f,  0.0f,  0.0f);
const Vector4 Vector4::forward			= Vector4( 0.0f,  0.0f,  1.0f,  0.0f);
const Vector4 Vector4::back				= Vector4( 0.0f,  0.0f, -1.0f,  0.0f);
const Vector4 Vector4::left				= Vector4(-1.0f,  0.0f,  0.0f,  0.0f);
const Vector4 Vector4::right			= Vector4( 1.0f,  0.0f,  0.0f,  0.0f);
const Vector4 Vector4::up				= Vector4( 0.0f,  1.0f,  0.0f,  0.0f);
const Vector4 Vector4::down				= Vector4( 0.0f, -1.0f,  0.0f,  0.0f);
const Vector4 Vector4::reverseOpaque	= Vector4( 0.0f,  0.0f,  0.0f, -1.0f);
const Vector4 Vector4::opaque			= Vector4( 0.0f,  0.0f,  0.0f,  1.0f);
const Vector4 Vector4::identity			= Vector4( 0.0f,  0.0f,  0.0f,  1.0f);
const Vector4 Vector4::one				= Vector4( 1.0f,  1.0f,  1.0f,  1.0f);

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

float Vector::GetLength(const XMVECTOR& vec)
{
	// 벡터의 각 성분을 제곱
	XMVECTOR vSquared = XMVectorMultiply(vec, vec);

	// 첫 세 성분의 합으로 제곱 길이 계산
	float lenSq = _mm_cvtss_f32(_mm_dp_ps(vSquared, _mm_setr_ps(1.f, 1.f, 1.f, 0.f), 0x71));

	// 제곱 길이의 근사 역제곱근 계산 (_mm_rsqrt_ss 사용)
	__m128 lenSqVec = _mm_set_ss(lenSq);
	__m128 approxInvSqrt = _mm_rsqrt_ss(lenSqVec);
	float invSqrt = _mm_cvtss_f32(approxInvSqrt);

	// 뉴턴-랩슨 보정: 정확도를 높이기 위한 1회 반복
	invSqrt = invSqrt * (1.5f - 0.5f * lenSq * invSqrt * invSqrt);

	// 최종 벡터 길이 계산: length = lenSq * (1/√lenSq)
	return lenSq * invSqrt;
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

Vector3 Vector::ConvertVector3(const DirectX::XMFLOAT3& vector3)
{
	return { vector3.x, vector3.y, vector3.z };
}

XMVECTOR Vector::ConvertXMVECTOR(const Vector3& vector3)
{
	return XMVectorSet(vector3.x, vector3.y, vector3.z, 0.0f);
}

XMVECTOR Vector::ConvertXMVECTOR(const Vector4& vector4)
{
	return XMVectorSet(vector4.x, vector4.y, vector4.z, vector4.w);
}

Quaternion Vector::ConvertQuaternion(const Vector3& eulerAngles)
{
    // eulerAngles를 라디안 단위의 (pitch, yaw, roll)로 해석
    XMFLOAT4 result;
    XMStoreFloat4(&result, XMQuaternionRotationRollPitchYaw(
        eulerAngles.x,  // pitch (X축 회전)
        eulerAngles.y,  // yaw   (Y축 회전)
        eulerAngles.z   // roll  (Z축 회전)
    ));
    
    return Quaternion(result.x, result.y, result.z, result.w);
}

Euler Vector::ConvertEuler(const Quaternion& quaternion)
{
    // 쿼터니언 정규화 (추가)
    XMVECTOR q = XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    q = XMQuaternionNormalize(q);
    
    // 회전 행렬로 변환
    XMMATRIX rotMatrix = XMMatrixRotationQuaternion(q);
    
    float pitch, yaw, roll;
    
    // 회전 행렬의 요소들
    float m11 = XMVectorGetX(rotMatrix.r[0]);
    float m12 = XMVectorGetY(rotMatrix.r[0]);
    float m13 = XMVectorGetZ(rotMatrix.r[0]);
    float m21 = XMVectorGetX(rotMatrix.r[1]);
    float m22 = XMVectorGetY(rotMatrix.r[1]);
    float m23 = XMVectorGetZ(rotMatrix.r[1]);
    float m31 = XMVectorGetX(rotMatrix.r[2]);
    float m32 = XMVectorGetY(rotMatrix.r[2]);
    float m33 = XMVectorGetZ(rotMatrix.r[2]);
    
    // 짐벌락 감지 및 개선된 각도 추출
    if (abs(m32) > 0.99999f) 
	{
        // 짐벌락 발생 - 특수 처리
        pitch = XM_PIDIV2 * (m32 > 0 ? 1 : -1);
        yaw = atan2f(-m13, m11);  // roll + yaw의 조합
        roll = 0;  // roll을 0으로 고정하고 모든 회전을 yaw에 할당
    } 
	
	else 
	{
        // 일반적인 경우
        pitch = asinf(-m32);
        yaw = atan2f(m31, m33);
        roll = atan2f(m12, m22);
    }
    
    // 원래 함수와 일관된 순서로 반환
    return Vector3(pitch, yaw, roll);
}

Quaternion Vector::GetQuaternion(const DirectX::XMFLOAT4X4& matrix)
{
	XMFLOAT4 result;
	XMStoreFloat4(&result, XMQuaternionRotationMatrix(XMLoadFloat4x4(&matrix)));
	
	return Vector4(result.x, result.y, result.z, result.w);
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

bool Vector3::operator==(const Vector2& vector) const
{
	return (x == vector.x && y == vector.y);
}

bool Vector3::operator==(const Vector3& vector) const
{
	return (x == vector.x && y == vector.y && z == vector.z);
}

bool Vector3::operator==(const Vector4& vector) const
{
	return (x == vector.x && y == vector.y && z == vector.z);
}

physx::PxVec3 Vector3::ConvertPxVec3(const Vector3& vector3)
{
	return physx::PxVec3(vector3.x, vector3.y, vector3.z);
}

float Vector3::GetLength() const
{
	return sqrtf(x * x + y * y + z * z);
}

bool Vector3::Equal(const Vector3& left, const Vector3& right, float epsilon)
{
	return (fabs(left.x - right.x) < epsilon) &&
		   (fabs(left.y - right.y) < epsilon) &&
		   (fabs(left.z - right.z) < epsilon);
}

bool Vector3::Eqaul(const DirectX::XMFLOAT3& left, const DirectX::XMFLOAT3& right, float epsilon)
{
	return (fabs(left.x - right.x) < epsilon) &&
		   (fabs(left.y - right.y) < epsilon) &&
		   (fabs(left.z - right.z) < epsilon);
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

bool Vector2::operator==(const Vector2& vector) const
{
	return (x == vector.x && y == vector.y);
}

bool Vector2::operator==(const Vector3& vector) const
{
	return (x == vector.x && y == vector.y);
}

bool Vector2::operator==(const Vector4& vector) const
{
	return (x == vector.x && y == vector.y);
}

float Vector2::GetLength() const
{
	return sqrtf(x * x + y * y);
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

bool Vector4::operator==(const Vector2& vector) const
{
	return (x == vector.x && y == vector.y);
}

bool Vector4::operator==(const Vector3& vector) const
{
	return (x == vector.x && y == vector.y && z == vector.z);
}

bool Vector4::operator==(const Vector4& vector) const
{
	return (x == vector.x && y == vector.y && z == vector.z && w == vector.w);
}

float Vector4::GetLength() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
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
