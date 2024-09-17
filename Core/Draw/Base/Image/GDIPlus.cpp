//#define FULL_WINTARD
#include "stdafx.h"
#include "GDIPlus.h"

namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <gdiplus.h>

ULONG_PTR GDIPlus::token = 0;
int GDIPlus::count = 0;

GDIPlus::GDIPlus()
{
	if (count++ == 0)
	{
		Gdiplus::GdiplusStartupInput input;
		input.GdiplusVersion = 1;
		input.DebugEventCallback = nullptr;
		input.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&token, &input, nullptr);
	}
}

GDIPlus::~GDIPlus()
{
	if (--count == 0)
		Gdiplus::GdiplusShutdown(token);
}