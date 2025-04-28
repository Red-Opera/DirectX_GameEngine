#pragma once

#include <memory>

namespace Engine
{
	class MenuBar
	{
	public:
		void RenderMenuBar();

		static void GetInstance()
		{
			if (!menuBar)
				menuBar = std::make_unique<MenuBar>();
		}

		static std::unique_ptr<MenuBar> menuBar;
	private:
	};
}