#include "stdafx.h"
#include "Matrix.h"

#include <cmath>
#include <DirectXMath.h>

#include <xmmintrin.h>

bool Matrix::Equal(const DirectX::XMMATRIX& a, const DirectX::XMMATRIX& b, float epsilon)
{
	// 오차 허용 범위를 벡터로 설정
    __m128 eps = _mm_set1_ps(epsilon);

    for (int i = 0; i < 4; ++i)
    {
		__m128 diff = _mm_sub_ps(a.r[i], b.r[i]);       // 각 행의 차이 계산
		diff = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff); // 절대값 계산 (부호 비트 제거 : -0.0f)

		// 비교: 차이가 epsilon보다 큰지 확인
        __m128 cmp = _mm_cmpgt_ps(diff, eps);

		// 하나라도 차이가 epsilon보다 크면 false 반환
        if (_mm_movemask_ps(cmp))
            return false;
    }

    return true;
}


bool Matrix::Equal(const DirectX::XMFLOAT4X4& a, const DirectX::XMFLOAT4X4& b, float epsilon)
{
    // 구조체의 각 멤버를 직접 비교
    return (std::abs(a._11 - b._11) <= epsilon) && (std::abs(a._12 - b._12) <= epsilon) &&
           (std::abs(a._13 - b._13) <= epsilon) && (std::abs(a._14 - b._14) <= epsilon) &&
           (std::abs(a._21 - b._21) <= epsilon) && (std::abs(a._22 - b._22) <= epsilon) &&
           (std::abs(a._23 - b._23) <= epsilon) && (std::abs(a._24 - b._24) <= epsilon) &&
           (std::abs(a._31 - b._31) <= epsilon) && (std::abs(a._32 - b._32) <= epsilon) &&
           (std::abs(a._33 - b._33) <= epsilon) && (std::abs(a._34 - b._34) <= epsilon) &&
           (std::abs(a._41 - b._41) <= epsilon) && (std::abs(a._42 - b._42) <= epsilon) &&
           (std::abs(a._43 - b._43) <= epsilon) && (std::abs(a._44 - b._44) <= epsilon);
}
