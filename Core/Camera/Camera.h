#pragma once

#include "CameraProjection.h"
#include "CameraIndicator.h"

#include <DirectXMath.h>
#include <string>

namespace RenderGraphNameSpace { class RenderGraph; }

class DxGraphic;

class Camera
{
public:
	Camera(DxGraphic& graphic, std::string name, DirectX::XMFLOAT3 initPosition = { -22.0f, 4.0f, 0.0f }, float initPitch = 0.0f, float initYaw = 1.57f, bool isTethered = false) noexcept;

	DirectX::XMMATRIX GetMatrix() const noexcept;			// ī�޶��� Matrix�� ��ȯ��
	DirectX::XMMATRIX GetProjection() const noexcept;
	
	void SpawnControlWidgets(DxGraphic& graphic) noexcept;	// ī�޶� ������ �� �ִ� ImGui�� ������
	void Reset(DxGraphic& graphic) noexcept;				// ī�޶� ��ġ �ʱ�ȭ
	
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;

	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
	DirectX::XMFLOAT3 GetPosition() const noexcept;			// ī�޶� ��ġ�� ��ȯ��
	const std::string& GetName() const noexcept;

	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);
	void Submit(size_t channel) const;

	void RenderToGraphic(DxGraphic& graphic) const;

private:
	std::string name;

	CameraIndicator indicator;
	CameraProjection projection;

	DirectX::XMFLOAT3 position;	// ī�޶� ��ġ
	DirectX::XMFLOAT3 initPosition;

	// ī�޶� ȸ��
	float pitch = 0.0f;
	float yaw = 0.0f;

	float initPitch;
	float initYaw;

	static constexpr float moveSpeed = 12.0f;		// ī�޶� �̵� �ӵ�
	static constexpr float rotationSpeed = 0.0004f;	// ī�޶� ȸ�� �ӵ�

	bool isEnableIndicator = false;
	bool isEnableFrustumIndicator = false;
	bool isTethered;
};