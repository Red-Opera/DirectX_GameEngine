#include "stdafx.h"
#include "Core/Window.h"
#include "App.h"

int CALLBACK wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow)
{
	try
	{ return App().Run(); }
	
	catch (const BaseException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}

	catch (const exception& e)
	{
		MessageBox(nullptr, e.what(), "표준 예외", MB_OK | MB_ICONEXCLAMATION);
	}

	catch (...)
	{
		MessageBox(nullptr, "세부적인 내용을 알 수 없음", "알 수 없는 예외", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}