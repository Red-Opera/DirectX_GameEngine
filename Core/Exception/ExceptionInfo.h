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

	void Set() noexcept;
	const char* GetMessages();

private:
	// 메시지 정보를 저장하는 구조체
	struct MessageInfo
	{
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity;
		const char* description;
		size_t length;
	};

	unsigned long long next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> infoQueue;
	
	// 메시지를 저장할 정적 버퍼 (최대 64KB)
	static constexpr size_t MESSAGE_BUFFER_SIZE = 65536;
	char messageBuffer[MESSAGE_BUFFER_SIZE];
	
	// 임시 메시지 저장 벡터 (정렬용)
	static constexpr size_t MAX_MESSAGES = 100;
	MessageInfo messageInfos[MAX_MESSAGES];
	
	// 캐싱된 메시지 (변경이 없으면 재사용)
	unsigned long long lastEnd = 0u;
	bool hasNewMessages = false;

	// 심각도에 따른 문자열 반환
	const char* GetSeverityString(DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity) const noexcept;
	
	// 심각도 우선순위 (낮을수록 높은 우선순위)
	int GetSeverityPriority(DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity) const noexcept;
};