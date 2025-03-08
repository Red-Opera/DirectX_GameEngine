#pragma once

#include "Base/Drawable.h"
#include <random>

template<class T>
class TestObject : public Drawable
{
public:
	TestObject(std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist) :
		r(rdist(random)), deltaRoll(ddist(random)), deltaPitch(ddist(random)), deltaYaw(ddist(random)),
		deltaTheta(odist(random)), deltaPhi(odist(random)), deltaChi(odist(random)), chi(adist(random)), theta(adist(random)), phi(adist(random)) { }

	void Update(float dt) noexcept
	{
		roll = Math::NormalizeRadian(roll +  deltaRoll * dt);
		pitch = Math::NormalizeRadian(pitch + deltaPitch * dt);
		yaw = Math::NormalizeRadian(yaw + deltaYaw * dt);

		theta = Math::NormalizeRadian(theta + deltaTheta * dt);
		phi = Math::NormalizeRadian(phi + deltaPhi * dt);
		chi = Math::NormalizeRadian(chi + deltaChi * dt);
	}

	DirectX::XMMATRIX GetTransformMatrix() const noexcept
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
};

