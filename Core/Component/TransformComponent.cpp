#include "stdafx.h"
#include "TransformComponent.h"

#include "Core/Object/Object.h"

TransformComponent::TransformComponent(std::shared_ptr<Object> object)
	: Component(object)
{

}

void TransformComponent::SetPosition(Position position) noexcept
{
	transform.position = position;

	// 부모가 있는 경우 로컬 위치 계산
	if (HasParent())
	{
		XMFLOAT3 worldPostiion = { position.x, position.y, position.z };

		// 부모의 역변환을 적용하여 로컬 위치 계산
		DirectX::XMVECTOR worldPos = DirectX::XMLoadFloat3(&worldPostiion);
		DirectX::XMMATRIX parentWorldToLocalMatrix = DirectX::XMMatrixInverse(nullptr, parent->GetTransformMatrix());
		DirectX::XMVECTOR localPos = DirectX::XMVector3TransformCoord(worldPos, parentWorldToLocalMatrix);

		DirectX::XMFLOAT3 localPosFloat3;
		DirectX::XMStoreFloat3(&localPosFloat3, localPos);

		localTransform.position = { localPosFloat3.x, localPosFloat3.y, localPosFloat3.z };
	}

	// 부모가 없는 경우 월드 위치와 로컬 위치는 같음
	else
		localTransform.position = position;
}

void TransformComponent::SetPosition(DirectX::XMFLOAT3 position) noexcept
{
	transform.position = { position.x, position.y, position.z };

	// 부모가 있는 경우 로컬 위치 계산
	if (HasParent())
	{
		// 부모의 역변환을 적용하여 로컬 위치 계산
		DirectX::XMVECTOR worldPos = DirectX::XMLoadFloat3(&position);
		DirectX::XMMATRIX parentWorldToLocalMatrix = DirectX::XMMatrixInverse(nullptr, parent->GetTransformMatrix());
		DirectX::XMVECTOR localPos = DirectX::XMVector3TransformCoord(worldPos, parentWorldToLocalMatrix);

		DirectX::XMFLOAT3 localPosFloat3;
		DirectX::XMStoreFloat3(&localPosFloat3, localPos);

		localTransform.position = { localPosFloat3.x, localPosFloat3.y, localPosFloat3.z };
	}

	// 부모가 없는 경우 월드 위치와 로컬 위치는 같음
	else
		localTransform.position = { position.x, position.y, position.z };
}

void TransformComponent::SetPosition(float x, float y, float z) noexcept
{
	transform.position = { x, y, z };

	// 부모가 있는 경우 로컬 위치 계산
	if (HasParent())
	{
		// 부모의 역변환을 적용하여 로컬 위치 계산
		DirectX::XMFLOAT3 worldPos = { x, y, z };
		DirectX::XMVECTOR worldPosVector = DirectX::XMLoadFloat3(&worldPos);
		DirectX::XMMATRIX parentWorldToLocalMatrix = DirectX::XMMatrixInverse(nullptr, parent->GetTransformMatrix());
		DirectX::XMVECTOR localPos = DirectX::XMVector3TransformCoord(worldPosVector, parentWorldToLocalMatrix);

		DirectX::XMFLOAT3 localPosFloat3;
		DirectX::XMStoreFloat3(&localPosFloat3, localPos);

		localTransform.position = { localPosFloat3.x, localPosFloat3.y, localPosFloat3.z };
	}

	// 부모가 없는 경우 월드 위치와 로컬 위치는 같음
	else
		localTransform.position = { x, y, z };
}

Position& TransformComponent::GetPosition() noexcept
{
	return transform.position;
}

void TransformComponent::SetRotation(float roll, float pitch, float yaw) noexcept
{
	transform.rotation = { roll, pitch, yaw };

	// 부모가 있는 경우 로컬 회전 계산
	if (HasParent())
	{
		// 월드 회전에서 부모의 회전을 빼서 로컬 회전 계산
		DirectX::XMVECTOR worldRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
		DirectX::XMVECTOR parentRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(parent->GetRotation().x, parent->GetRotation().y, parent->GetRotation().z);
		DirectX::XMVECTOR parentRotQuatInv = DirectX::XMQuaternionInverse(parentRotQuat);
		DirectX::XMVECTOR localRotQuat = DirectX::XMQuaternionMultiply(parentRotQuatInv, worldRotQuat);

		// 쿼터니언을 오일러 각으로 변환
		DirectX::XMFLOAT4 localRotFloat4;
		DirectX::XMStoreFloat4(&localRotFloat4, localRotQuat);

		// 쿼터니언에서 오일러 각 추출
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(localRotQuat);
		float localPitch = asinf(-rotMatrix.r[2].m128_f32[1]);
		float localRoll, localYaw;

		if (cosf(localPitch) > 0.0001f)
		{
			localRoll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
			localYaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
		}
		else
		{
			localRoll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
			localYaw = 0.0f;
		}

		localTransform.rotation = { localRoll, localPitch, localYaw };
	}

	// 부모가 없는 경우 월드 회전과 로컬 회전은 같음
	else
		localTransform.rotation = { roll, pitch, yaw };
}

Rotation& TransformComponent::GetRotation() noexcept
{
	return transform.rotation;
}

void TransformComponent::SetScale(Scale scale) noexcept
{
	transform.scale = scale;

	// 부모가 있는 경우 로컬 스케일 계산
	if (HasParent())
	{
		// 월드 스케일을 부모의 스케일로 나누어 로컬 스케일 계산
		localTransform.scale = {
			scale.x / parent->GetScale().x,
			scale.y / parent->GetScale().y,
			scale.z / parent->GetScale().z
		};
	}

	// 부모가 없는 경우 월드 스케일과 로컬 스케일은 같음
	else
		localTransform.scale = scale;
}

void TransformComponent::SetScale(DirectX::XMFLOAT3 scale) noexcept
{
	transform.scale = { scale.x, scale.y, scale.z };

	// 부모가 있는 경우 로컬 스케일 계산
	if (HasParent())
	{
		// 월드 스케일을 부모의 스케일로 나누어 로컬 스케일 계산
		localTransform.scale = {
			scale.x / parent->GetScale().x,
			scale.y / parent->GetScale().y,
			scale.z / parent->GetScale().z
		};
	}

	// 부모가 없는 경우 월드 스케일과 로컬 스케일은 같음
	else
		localTransform.scale = { scale.x, scale.y, scale.z };
}

void TransformComponent::SetScale(float x, float y, float z) noexcept
{
	transform.scale = { x, y, z };

	// 부모가 있는 경우 로컬 스케일 계산
	if (HasParent())
	{
		// 월드 스케일을 부모의 스케일로 나누어 로컬 스케일 계산
		localTransform.scale = {
			x / parent->GetScale().x,
			y / parent->GetScale().y,
			z / parent->GetScale().z
		};
	}
	// 부모가 없는 경우 월드 스케일과 로컬 스케일은 같음
	else
		localTransform.scale = { x, y, z };
}

Scale& TransformComponent::GetScale() noexcept
{
	return transform.scale;
}

const Vector3 TransformComponent::GetRight() const noexcept
{
	// 회전 행렬을 생성
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	// 월드 좌표계의 기본 오른쪽 벡터 (1, 0, 0)를 로드
	DirectX::XMVECTOR rightVector = DirectX::XMLoadFloat3(&Vector::right);

	// 회전 행렬을 사용하여 기본 오른쪽 벡터를 변환
	rightVector = DirectX::XMVector3TransformNormal(rightVector, rotationMatrix);

	// 변환된 벡터를 정규화
	rightVector = DirectX::XMVector3Normalize(rightVector);

	// XMVECTOR를 Vector3로 변환
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, rightVector);

	return Vector3(right.x, right.y, right.z);
}

const Vector3 TransformComponent::GetUp() const noexcept
{
	// 회전 행렬을 생성
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	// 월드 좌표계의 기본 오른쪽 벡터 (1, 0, 0)를 로드
	DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&Vector::up);

	// 회전 행렬을 사용하여 기본 오른쪽 벡터를 변환
	upVector = DirectX::XMVector3TransformNormal(upVector, rotationMatrix);

	// 변환된 벡터를 정규화
	upVector = DirectX::XMVector3Normalize(upVector);

	// XMVECTOR를 Vector3로 변환
	DirectX::XMFLOAT3 up;
	DirectX::XMStoreFloat3(&up, upVector);

	return Vector3(up.x, up.y, up.z);
}

const Vector3 TransformComponent::GetForward() const noexcept
{
	// 회전 행렬을 생성
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	// 월드 좌표계의 기본 오른쪽 벡터 (1, 0, 0)를 로드
	DirectX::XMVECTOR forwardVector = DirectX::XMLoadFloat3(&Vector::forward);

	// 회전 행렬을 사용하여 기본 오른쪽 벡터를 변환
	forwardVector = DirectX::XMVector3TransformNormal(forwardVector, rotationMatrix);

	// 변환된 벡터를 정규화
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	// XMVECTOR를 Vector3로 변환
	DirectX::XMFLOAT3 forward;
	DirectX::XMStoreFloat3(&forward, forwardVector);

	return Vector3(forward.x, forward.y, forward.z);
}

Transform& TransformComponent::GetTransform() noexcept
{
	return transform;
}

void TransformComponent::SetRotation(Rotation rotation) noexcept
{
	transform.rotation = rotation;

	// 부모가 있는 경우 로컬 회전 계산
	if (HasParent())
	{
		// 월드 회전에서 부모의 회전을 빼서 로컬 회전 계산
		// 이때 쿼터니언 방식으로 변환하여 계산
		DirectX::XMVECTOR worldRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMVECTOR parentRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(parent->GetRotation().x, parent->GetRotation().y, parent->GetRotation().z);
		DirectX::XMVECTOR parentRotQuatInv = DirectX::XMQuaternionInverse(parentRotQuat);
		DirectX::XMVECTOR localRotQuat = DirectX::XMQuaternionMultiply(parentRotQuatInv, worldRotQuat);

		// 쿼터니언을 오일러 각으로 변환
		DirectX::XMFLOAT4 localRotFloat4;
		DirectX::XMStoreFloat4(&localRotFloat4, localRotQuat);

		// 쿼터니언에서 오일러 각 추출
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(localRotQuat);
		float pitch = asinf(-rotMatrix.r[2].m128_f32[1]);
		float roll, yaw;

		if (cosf(pitch) > 0.0001f)
		{
			roll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
			yaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
		}

		else
		{
			roll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
			yaw = 0.0f;
		}

		localTransform.rotation = { roll, pitch, yaw };
	}

	// 부모가 없는 경우 월드 회전과 로컬 회전은 같음
	else
		localTransform.rotation = rotation;
}

void TransformComponent::SetRotation(DirectX::XMFLOAT3 rotation) noexcept
{
	transform.rotation = { rotation.x, rotation.y, rotation.z };

	// 부모가 있는 경우 로컬 회전 계산
	if (HasParent())
	{
		// 월드 회전에서 부모의 회전을 빼서 로컬 회전 계산
		DirectX::XMVECTOR worldRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMVECTOR parentRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(parent->GetRotation().x, parent->GetRotation().y, parent->GetRotation().z);
		DirectX::XMVECTOR parentRotQuatInv = DirectX::XMQuaternionInverse(parentRotQuat);
		DirectX::XMVECTOR localRotQuat = DirectX::XMQuaternionMultiply(parentRotQuatInv, worldRotQuat);

		// 쿼터니언을 오일러 각으로 변환
		DirectX::XMFLOAT4 localRotFloat4;
		DirectX::XMStoreFloat4(&localRotFloat4, localRotQuat);

		// 쿼터니언에서 오일러 각 추출
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(localRotQuat);
		float pitch = asinf(-rotMatrix.r[2].m128_f32[1]);
		float roll, yaw;

		if (cosf(pitch) > 0.0001f)
		{
			roll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
			yaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
		}

		else
		{
			roll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
			yaw = 0.0f;
		}

		localTransform.rotation = { roll, pitch, yaw };
	}

	// 부모가 없는 경우 월드 회전과 로컬 회전은 같음
	else
		localTransform.rotation = { rotation.x, rotation.y, rotation.z };
}

void TransformComponent::SetLocalPosition(Position position) noexcept
{
	localTransform.position = position;

	// 부모가 있는 경우 월드 위치 계산
	if (HasParent())
	{
		// 부모의 변환 행렬을 사용하여 월드 위치 계산
		DirectX::XMFLOAT3 localPos = { position.x, position.y, position.z };
		DirectX::XMVECTOR localPosVector = DirectX::XMLoadFloat3(&localPos);
		DirectX::XMMATRIX parentWorldMatrix = parent->GetTransformMatrix();
		DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(localPosVector, parentWorldMatrix);

		DirectX::XMFLOAT3 worldPosFloat3;
		DirectX::XMStoreFloat3(&worldPosFloat3, worldPos);

		transform.position = { worldPosFloat3.x, worldPosFloat3.y, worldPosFloat3.z };
	}

	// 부모가 없는 경우 월드 위치와 로컬 위치는 같음
	else
		transform.position = position;
}

void TransformComponent::SetLocalPosition(float x, float y, float z) noexcept
{
	localTransform.position = { x, y, z };

	// 부모가 있는 경우 월드 위치 계산
	if (HasParent())
	{
		// 부모의 변환 행렬을 사용하여 월드 위치 계산
		DirectX::XMFLOAT3 localPos = { x, y, z };
		DirectX::XMVECTOR localPosVector = DirectX::XMLoadFloat3(&localPos);
		DirectX::XMMATRIX parentWorldMatrix = parent->GetTransformMatrix();
		DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(localPosVector, parentWorldMatrix);

		DirectX::XMFLOAT3 worldPosFloat3;
		DirectX::XMStoreFloat3(&worldPosFloat3, worldPos);

		transform.position = { worldPosFloat3.x, worldPosFloat3.y, worldPosFloat3.z };
	}

	// 부모가 없는 경우 월드 위치와 로컬 위치는 같음
	else
		transform.position = { x, y, z };
}

void TransformComponent::SetLocalRotation(float roll, float pitch, float yaw) noexcept
{
	localTransform.rotation = { roll, pitch, yaw };

	// 부모가 있는 경우 월드 회전 계산
	if (HasParent())
	{
		// 로컬 회전을 월드 회전으로 변환
		DirectX::XMVECTOR localRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
		DirectX::XMVECTOR parentRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(parent->GetRotation().x, parent->GetRotation().y, parent->GetRotation().z);
		DirectX::XMVECTOR worldRotQuat = DirectX::XMQuaternionMultiply(localRotQuat, parentRotQuat);

		// 쿼터니언에서 오일러 각 계산
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(worldRotQuat);
		float worldPitch = asinf(-rotMatrix.r[2].m128_f32[1]);
		float worldRoll, worldYaw;

		if (cosf(worldPitch) > 0.0001f)
		{
			worldRoll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
			worldYaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
		}
		else
		{
			worldRoll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
			worldYaw = 0.0f;
		}

		transform.rotation = { worldRoll, worldPitch, worldYaw };
	}

	// 부모가 없는 경우 월드 회전과 로컬 회전은 같음
	else
		transform.rotation = { roll, pitch, yaw };
}

void TransformComponent::SetLocalScale(Scale scale) noexcept
{
	localTransform.scale = scale;

	// 부모가 있는 경우 월드 스케일 계산
	if (HasParent())
	{
		// 로컬 스케일에 부모의 스케일을 곱하여 월드 스케일 계산
		transform.scale = {
			scale.x * parent->GetScale().x,
			scale.y * parent->GetScale().y,
			scale.z * parent->GetScale().z
		};
	}

	// 부모가 없는 경우 월드 스케일과 로컬 스케일은 같음
	else
		transform.scale = scale;
}

void TransformComponent::SetLocalScale(DirectX::XMFLOAT3 scale) noexcept
{
	localTransform.scale = { scale.x, scale.y, scale.z };

	// 부모가 있는 경우 월드 스케일 계산
	if (HasParent())
	{
		// 로컬 스케일에 부모의 스케일을 곱하여 월드 스케일 계산
		transform.scale = {
			scale.x * parent->GetScale().x,
			scale.y * parent->GetScale().y,
			scale.z * parent->GetScale().z
		};
	}

	// 부모가 없는 경우 월드 스케일과 로컬 스케일은 같음
	else
		transform.scale = { scale.x, scale.y, scale.z };
}

void TransformComponent::SetLocalScale(float x, float y, float z) noexcept
{
	localTransform.scale = { x, y, z };

	// 부모가 있는 경우 월드 스케일 계산
	if (HasParent())
	{
		// 로컬 스케일에 부모의 스케일을 곱하여 월드 스케일 계산
		transform.scale = {
			x * parent->GetScale().x,
			y * parent->GetScale().y,
			z * parent->GetScale().z
		};
	}

	// 부모가 없는 경우 월드 스케일과 로컬 스케일은 같음
	else
		transform.scale = { x, y, z };
}

Transform& TransformComponent::GetLocalTransform() noexcept
{
	return localTransform;
}

Position& TransformComponent::GetLocalPosition() noexcept
{
	return localTransform.position;
}

Rotation& TransformComponent::GetLocalRotation() noexcept
{
	return localTransform.rotation;
}

Scale& TransformComponent::GetLocalScale() noexcept
{
	return localTransform.scale;
}

void TransformComponent::SetLocalRotation(Rotation rotation) noexcept
{
	localTransform.rotation = rotation;

	// 부모가 있는 경우 월드 회전 계산
	if (HasParent())
	{
		// 로컬 회전을 월드 회전으로 변환
		DirectX::XMVECTOR localRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMVECTOR parentRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(parent->GetRotation().x, parent->GetRotation().y, parent->GetRotation().z);
		DirectX::XMVECTOR worldRotQuat = DirectX::XMQuaternionMultiply(localRotQuat, parentRotQuat);

		// 쿼터니언에서 오일러 각 계산
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(worldRotQuat);
		float pitch = asinf(-rotMatrix.r[2].m128_f32[1]);
		float roll, yaw;

		if (cosf(pitch) > 0.0001f)
		{
			roll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
			yaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
		}
		else
		{
			roll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
			yaw = 0.0f;
		}

		transform.rotation = { roll, pitch, yaw };
	}

	// 부모가 없는 경우 월드 회전과 로컬 회전은 같음
	else
		transform.rotation = rotation;
}

void TransformComponent::SetLocalRotation(DirectX::XMFLOAT3 rotation) noexcept
{
	localTransform.rotation = { rotation.x, rotation.y, rotation.z };

	// 부모가 있는 경우 월드 회전 계산
	if (HasParent())
	{
		// 로컬 회전을 월드 회전으로 변환
		DirectX::XMVECTOR localRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMVECTOR parentRotQuat = DirectX::XMQuaternionRotationRollPitchYaw(parent->GetRotation().x, parent->GetRotation().y, parent->GetRotation().z);
		DirectX::XMVECTOR worldRotQuat = DirectX::XMQuaternionMultiply(localRotQuat, parentRotQuat);

		// 쿼터니언에서 오일러 각 계산
		DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(worldRotQuat);
		float pitch = asinf(-rotMatrix.r[2].m128_f32[1]);
		float roll, yaw;

		if (cosf(pitch) > 0.0001f)
		{
			roll = atan2f(rotMatrix.r[2].m128_f32[0], rotMatrix.r[2].m128_f32[2]);
			yaw = atan2f(rotMatrix.r[0].m128_f32[1], rotMatrix.r[1].m128_f32[1]);
		}

		else
		{
			roll = atan2f(-rotMatrix.r[0].m128_f32[2], rotMatrix.r[0].m128_f32[0]);
			yaw = 0.0f;
		}

		transform.rotation = { roll, pitch, yaw };
	}

	// 부모가 없는 경우 월드 회전과 로컬 회전은 같음
	else
		transform.rotation = { rotation.x, rotation.y, rotation.z };
}

DirectX::XMMATRIX TransformComponent::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z) *
		DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);
}

DirectX::XMFLOAT4X4& TransformComponent::GetTransformMatrix4x4() noexcept
{
	DirectX::XMMATRIX newTransformMatrix = DirectX::XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z) *
		DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);

	DirectX::XMStoreFloat4x4(&transformMatrix, newTransformMatrix);

	return transformMatrix;
}

DirectX::XMMATRIX TransformComponent::GetLocalTransformMatrix() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(localTransform.rotation.x, localTransform.rotation.y, localTransform.rotation.z) *
		DirectX::XMMatrixTranslation(localTransform.position.x, localTransform.position.y, localTransform.position.z);
}

DirectX::XMFLOAT4X4& TransformComponent::GetLocalTransformMatrix4x4() noexcept
{
	DirectX::XMMATRIX newTransformMatrix = DirectX::XMMatrixRotationRollPitchYaw(localTransform.rotation.x, localTransform.rotation.y, localTransform.rotation.z) *
		DirectX::XMMatrixTranslation(localTransform.position.x, localTransform.position.y, localTransform.position.z);

	DirectX::XMStoreFloat4x4(&transformMatrix, newTransformMatrix);

	return transformMatrix;
}

void TransformComponent::SetParent(std::shared_ptr<TransformComponent> parent) noexcept
{
	this->parent = parent;

	parent->children.push_back(this->shared_from_this());
}

void TransformComponent::SetParent(std::shared_ptr<Object> parent) noexcept
{
	std::shared_ptr<TransformComponent> parentComponent = parent->GetComponent<TransformComponent>();

	this->parent = parentComponent;

	parentComponent->children.push_back(this->shared_from_this());
}

bool TransformComponent::HasParent() const noexcept
{
	return parent != nullptr;
}

void TransformComponent::RemoveParent() noexcept
{
	if (parent == nullptr)
		return;

	parent->RemoveChild(this->shared_from_this());
	parent = nullptr;
}

void TransformComponent::AddChild(std::shared_ptr<TransformComponent> child) noexcept
{
	children.push_back(child);
	childIndex[child->GetObject()->GetName()] = ((UINT)children.size() - (UINT)1);

	child->parent = this->shared_from_this();
}

void TransformComponent::AddChild(std::shared_ptr<Object> child) noexcept
{
	std::shared_ptr<TransformComponent> childComponent = child->GetComponent<TransformComponent>();

	children.push_back(childComponent);
	childIndex[child->GetName()] = ((UINT)children.size() - (UINT)1);

	childComponent->parent = this->shared_from_this();
}

void TransformComponent::RemoveChild(std::shared_ptr<TransformComponent> child) noexcept
{
	auto iter = childIndex.find(child->GetObject()->GetName());

	if (iter != childIndex.end())
		return;

	UINT indexToRemove = iter->second;

	children.erase(children.begin() + indexToRemove);
	childIndex.erase(iter);

	for (size_t i = indexToRemove; i < children.size(); i++)
		childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

void TransformComponent::RemoveChild(UINT index) noexcept
{
	if (index >= children.size())
		return;

	children.erase(children.begin() + index);
}

void TransformComponent::RemoveChild(std::shared_ptr<Object> child) noexcept
{
	auto iter = childIndex.find(child->GetName());

	if (iter != childIndex.end())
		return;

	UINT indexToRemove = iter->second;

	children.erase(children.begin() + indexToRemove);

	childIndex.erase(iter);

	for (size_t i = indexToRemove; i < children.size(); i++)
		childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

void TransformComponent::RemoveChild(std::string childObjectName) noexcept
{
	auto iter = childIndex.find(childObjectName);

	if (iter != childIndex.end())
		return;

	UINT indexToRemove = iter->second;

	children.erase(children.begin() + indexToRemove);
	childIndex.erase(iter);

	for (size_t i = indexToRemove; i < children.size(); i++)
		childIndex[children[i]->GetObject()->GetName()] = (UINT)i;
}

bool TransformComponent::HasChild(std::shared_ptr<TransformComponent> child) const noexcept
{
	auto iter = childIndex.find(child->GetObject()->GetName());

	return iter != childIndex.end();
}

bool TransformComponent::HasChild(std::shared_ptr<Object> child) const noexcept
{
	auto iter = childIndex.find(child->GetName());

	return iter != childIndex.end();
}

bool TransformComponent::HasChild(std::string childObjectName) const noexcept
{
	auto iter = childIndex.find(childObjectName);

	return iter != childIndex.end();
}

bool TransformComponent::HasChild(UINT index) const noexcept
{
	return index < children.size();
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(std::shared_ptr<Object> child) noexcept
{
	if (HasChild(child) == false)
		return nullptr;

	return children[childIndex[child->GetName()]];
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(std::string childObjectName) noexcept
{
	if (HasChild(childObjectName) == false)
		return nullptr;

	return children[childIndex[childObjectName]];
}

std::shared_ptr<TransformComponent> TransformComponent::GetChild(UINT index) noexcept
{
	if (index >= children.size())
		return nullptr;

	return children[index];
}

std::vector<std::shared_ptr<TransformComponent>> TransformComponent::GetChildrens() noexcept
{
	return children;
}

size_t TransformComponent::GetChildCount() const noexcept
{
	return children.size();
}

void TransformComponent::UpdateTransform() noexcept
{
}
