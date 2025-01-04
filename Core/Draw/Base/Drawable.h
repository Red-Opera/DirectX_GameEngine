#pragma once

#include "../../DxGraphic.h"

#include <memory>
#include <DirectXMath.h>

namespace Graphic
{
	class Render;
	class IndexBuffer;
}

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;	// ������Ʈ�� Transform�� ����� ��ȯ
	void Draw(DxGraphic& graphic) const NOEXCEPTRELEASE;				// �ش� ������Ʈ�� �׸��� �޼ҵ�

	virtual ~Drawable() = default;

	template<class T>
	T* GetRenderObject() noexcept
	{
		for (auto& renderObject : renderObjects)
		{
			if (auto out = dynamic_cast<T*>(renderObject.get()))
				return out;
		}

		return nullptr;
	}

protected:
	void AddRender(std::shared_ptr<Graphic::Render> bind) NOEXCEPTRELEASE;

private:
	const Graphic::IndexBuffer* indexBuffer = nullptr;
	std::vector<std::shared_ptr<Graphic::Render>> renderObjects;
};

