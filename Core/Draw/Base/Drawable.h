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
	void AddBind(unique_ptr<Bindable> bind) noexcept (!_DEBUG);
	void AddIndexBuffer(unique_ptr<class IndexBuffer> indexBuffer) noexcept (!_DEBUG);

private:
	virtual const vector<unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

	const class IndexBuffer* indexBuffer = nullptr;
	vector<unique_ptr<Bindable>> binds;
};

