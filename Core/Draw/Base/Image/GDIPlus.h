#pragma once
#include "Core/Win.h"

class GDIPlus
{
public:
	GDIPlus();
	~GDIPlus();

private:
	static ULONG_PTR token;
	static int count;
};