#pragma once

#include "Core/Draw/Base/Drawable.h"

class CameraFrustum : public Drawable
{
public:
    CameraFrustum(float width, float height, float nearZ, float farZ);

    void SetVertices(float width, float height, float nearZ, float farZ);
    void SetPosition(Position position) noexcept;
    void SetRotation(Quaternion rotation) noexcept;

    DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
    Position position = Position::zero;
    Quaternion rotation = Quaternion::identity; // 항등 쿼터니언
};