#pragma once
#include "Drawable.h"
#include "Core/RenderingPipeline/IA/IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !staticBinds.empty();
	}

	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept (!_DEBUG)
	{
		assert("Index Buffer�� AddStaticIndexBuffer�� ���" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(move(bind));
	}

	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept (!_DEBUG)
	{
		assert("Index Buffer�� �������� �ʽ��ϴ�." && this->indexBuffer == nullptr);
		this->indexBuffer = indexBuffer.get();
		staticBinds.push_back(move(indexBuffer));
	}
	
	void SetIndexFromStatic() noexcept (!_DEBUG)
	{
		assert("Index Buffer�� �������� �ʽ��ϴ�." && indexBuffer == nullptr);

		for (const auto& b : staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				indexBuffer = p;
				return;
			}
		}

		assert("Index Buffer�� ���� ���ε� ����!" && indexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}

	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;