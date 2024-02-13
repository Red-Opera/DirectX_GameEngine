#pragma once
#include "Base/DrawableBase.h"

#include <random>

class DrawBox : public DrawableBase<DrawBox>
{
public:
	DrawBox(DxGraphic& graphic, std::mt19937& random,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	// Position Rotation
	float r;
	float roll = 0.0f;		// X�� ���� ȸ��
	float pitch = 0.0f;		// Y�� ���� ȸ��
	float yaw = 0.0f;		// Z�� ���� ȸ��

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

	DirectX::XMFLOAT3X3 transformMatrix;
};

