#pragma once

#include <math.h>

class Math
{
public:
	template<typename T>
	static constexpr auto Pow2(const T& x) noexcept { return x * x; }

	template<typename T>
	static constexpr T Lerp(const T& start, const T& end, float persent) noexcept { return start + (end - start) * persent; }

	// =====================================================
	//						Angle
	// =====================================================

	template<typename T>
	static constexpr T ConvertAngleToRadian(T angle) noexcept { return angle * PI / (T)180.0; }

	template<typename T>
	static constexpr T NormalizeRadian(T radian) noexcept
	{
		constexpr T oneCycle = (T)2 * (T)PIDOUBLE;
		const T currrentRadianFromOneCycle = (T)fmod(radian, oneCycle);

		// ������ PI�� �Ѵ� ���
		if (currrentRadianFromOneCycle > (T)PIDOUBLE)
			return currrentRadianFromOneCycle - oneCycle;

		else if (currrentRadianFromOneCycle < -(T)PIDOUBLE)
			return currrentRadianFromOneCycle + oneCycle;

		return currrentRadianFromOneCycle;
	}

	template<typename T>
	static constexpr T NormalizeAngle(T angle)
	{
		const T currentAngleFromOneCycle = (T)fmod(angle, 360);

		if (currentAngleFromOneCycle > (T)180.0)
			return currentAngleFromOneCycle - 360;

		else if (currentAngleFromOneCycle < -(T)180.0)
			return currentAngleFromOneCycle + 360;

		return currentAngleFromOneCycle;
	}

	// ����þ� �Լ� (���� ���� Ȯ�� �е� �Լ� �Ի�)
	template<typename T>
	static constexpr T Gaussian(T x, T sigma) noexcept
	{
		const auto sigmaSquare = Pow2(sigma);

		// ����ȭ ���
        const auto normalization = (T)1.0 / sqrt((T)2.0 * (T)PIDOUBLE * sigmaSquare);

		// ����ȭ ����
		const auto exponential = exp(-Pow2(x) / ((T)2.0 * sigmaSquare));

		return normalization * exponential;
	}

	static DirectX::XMMATRIX MultipleMatrixScale(DirectX::XMMATRIX matrix, float scale);

	static constexpr float PI = 3.1415926535f;
	static constexpr double PIDOUBLE = 3.141592653589793238;
};