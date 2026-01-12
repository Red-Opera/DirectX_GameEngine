#pragma once

#include "../Win.h"

#include <wrl.h>
#include <dxgidebug.h>

#include <vector>
#include <string>

class ExceptionInfo
{
public:
	ExceptionInfo();
	~ExceptionInfo() = default;
	ExceptionInfo(const ExceptionInfo&) = delete;
	ExceptionInfo& operator=(const ExceptionInfo&) = delete;

	static ExceptionInfo& GetCurrent();

	void Set() noexcept;
	const char* GetMessages();

private:
	// 메시지 정보를 저장하는 구조체
	struct MessageInfo
	{
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity;	// 메세지의 심각도
		const char* description;					// 메세지 내용
		size_t length;								// 메세지 길이
	};

	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> infoQueue;
	
	// 메시지를 저장할 정적 버퍼 (최대 4KB)
	static constexpr size_t maxLogLength = 4048;
	char messageBuffer[maxLogLength];
	
	// DXGI가 반환할 수 있는 최대 메시지 수
	static constexpr size_t maxMessageCount = 20;
	MessageInfo messageInfos[maxMessageCount];
	
	// 캐싱된 메시지 (변경이 없으면 재사용)
	unsigned long long lastEnd = 0u;
	bool hasNewMessages = false;

	// 심각도에 따른 문자열 반환
	const char* GetSeverityString(DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity) const noexcept;
	
	// 심각도 우선순위 (낮을수록 높은 우선순위)
	int GetSeverityPriority(DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity) const noexcept;
};