#pragma once

#include "Render.h"

#include "../DxGraphic.h"

#include <memory>
#include <type_traits>
#include <unordered_map>

namespace Graphic
{
	class RenderManager
	{
	public:
		template<class T, typename... Params>
		static std::shared_ptr<T> GetRender(DxGraphic& graphic, Params&&... path) NOEXCEPTRELEASE
		{
			static_assert(std::is_base_of<Render, T>::value, "�ش� Ŭ������ Rendering Pine������ ����� �ƴմϴ�.");

			return Instance().GetRender_<T>(graphic, std::forward<Params>(path)...);
		}

	private:
		// Key ���� �̿��Ͽ� RenderManager�� ����Ǿ� �ִ� Render ��ü�� ������
		template<class T, typename... Params>
		std::shared_ptr<T> GetRender_(DxGraphic& graphic, Params&&... path) NOEXCEPTRELEASE
		{
			const auto key = T::CreateID(std::forward<Params>(path)...);
			auto i = renders.find(key);	

			if (i == renders.end())
			{
				auto render = std::make_shared<T>(graphic, std::forward<Params>(path)...); 
				renders[key] = render;

				return render;
			}

			else
				return std::static_pointer_cast<T>(i->second);
		}

		static RenderManager& Instance()
		{
			static RenderManager renderManager;

			return renderManager;
		}

		std::unordered_map<std::string, std::shared_ptr<Render>> renders;
	};
}