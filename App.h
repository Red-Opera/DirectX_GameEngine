#pragma once

#include "Core/Window.h"

class App
{
public:
	App();
	~App();

	int Run();

private:
	void DoFrame();
	
	Window wnd;
	GameTimer timer;

	vector<unique_ptr<class Drawable>> drawables;
	static constexpr size_t drawableCount = 180;
};

