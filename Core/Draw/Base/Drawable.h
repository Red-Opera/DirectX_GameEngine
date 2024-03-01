#pragma once

#include "../../DxGraphic.h"

#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(DxGraphic& graphic) const noexcept (!_DEBUG);

	virtual void Update(float dt) noexcept = 0;
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

	void AddBind(std::unique_ptr<Bindable> bind) noexcept (!_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept (!_DEBUG);

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

	const class IndexBuffer* indexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};

