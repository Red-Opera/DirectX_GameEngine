#pragma once

#include "Base/DrawableBase.h"
#include <random>

template<class T>
class TestObject :  public DrawableBase<T>
{
public:
	TestObject(DxGraphic& graphic, std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist) :
		r(rdist(random)), deltaRoll(ddist(random)), deltaPitch(ddist(random)), deltaYaw(ddist(random)),
		deltaTheta(odist(random)), deltaPhi(odist(random)), deltaChi(odist(random)), chi(adist(random)), theta(adist(random)), phi(adist(random)) { }

	void Update(float dt) noexcept
	{
		roll = wrap_angle(roll +  deltaRoll * dt);
		pitch = wrap_angle(pitch + deltaPitch * dt);
		yaw = wrap_angle(yaw + deltaYaw * dt);

		theta = wrap_angle(theta + deltaTheta * dt);
		phi = wrap_angle(phi + deltaPhi * dt);
		chi = wrap_angle(chi + deltaChi * dt);
	}

	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
			   DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *
			   DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}

protected:
	// Position Rotation
	float r;
	float roll = 0.0f;		// X축 기준 회전
	float pitch = 0.0f;		// Y축 기준 회전
	float yaw = 0.0f;		// Z축 기준 회전

	float theta;
	float phi;
	float chi;

	// speed (deltaTime/s)
	float deltaRoll;
	float deltaPitch;
	float deltaYaw;

	float deltaTheta;
	float deltaPhi;
	float deltaChi;

private:
	template<typename T>
	T wrap_angle(T theta)
	{
		const T modded = fmod(theta, (T)2.0 * (T)PI);
		return (modded > (T)PI) ?
			(modded - (T)2.0 * (T)PI) :
			modded;
	}
};

