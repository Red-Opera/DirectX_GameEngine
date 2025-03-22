#pragma once

#include "../Win.h"

#include <vector>
#include <string>
#include <wrl.h>

class ExceptionInfo
{
public:
	ExceptionInfo();
	~ExceptionInfo() = default;

	ExceptionInfo(const ExceptionInfo&) = delete;
	ExceptionInfo& operator=(const ExceptionInfo&) = delete;

	void Set() noexcept; 
    const char* GetMessages();

private:
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<struct IDXGIInfoQueue> infoQueue;

	char message[100][128];
	UINT messageCount = 0u;
};

const GUID DXGI_DEBUG_ALL;