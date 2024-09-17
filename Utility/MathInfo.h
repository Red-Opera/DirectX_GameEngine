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
	static constexpr T ConvertAngleToRadian(T angle) { return angle * PI / (T)180.0; }

	template<typename T>
	static constexpr T NormalizeRadian(T radian)
	{
		constexpr T oneCycle = (T)2 * (T)PIDOUBLE;
		const T currrentRadianFromOneCycle = (T)fmod(radian, oneCycle);

		// 각도가 PI를 넘는 경우
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

	static constexpr float PI = 3.1415926535f;
	static constexpr double PIDOUBLE = 3.141592653589793238;
};