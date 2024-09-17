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

protected:
	template<class T>
	T* GetBindObject() noexcept
	{
		for (auto& bindObject : binds)
		{
			if (auto out = dynamic_cast<T*>(bindObject.get()))
				return out;
		}

		return nullptr;
	}

	void AddBind(std::shared_ptr<Graphic::Render> bind) NOEXCEPTRELEASE;

private:
	const Graphic::IndexBuffer* indexBuffer = nullptr;
	std::vector<std::shared_ptr<Graphic::Render>> binds;
};

