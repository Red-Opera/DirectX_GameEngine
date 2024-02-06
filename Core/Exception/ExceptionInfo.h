#pragma once

#include "../Win.h"

#include <vector>
#include <string>
#include <wrl.h>
using namespace std;

class ExceptionInfo
{
public:
	ExceptionInfo();
	~ExceptionInfo() = default;

	ExceptionInfo(const ExceptionInfo&) = delete;
	ExceptionInfo& operator=(const ExceptionInfo&) = delete;

	void Set() noexcept;
	vector<string> GetMessages() const;

private:
	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<struct IDXGIInfoQueue> infoQueue;
};

const GUID DXGI_DEBUG_ALL;