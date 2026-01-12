#include "stdafx.h"
#include "ExceptionInfo.h"
#include "../Window.h"
#include "../DxGraphic.h"

#include <dxgi1_3.h>
#include <dxgidebug.h>
#include <algorithm>

#pragma comment(lib, "dxguid.lib")

using namespace std;
using Microsoft::WRL::ComPtr;

ExceptionInfo::ExceptionInfo()
{
	// Windows SDK에서는 DXGIGetDebugInterface1을 직접 사용 가능
	HRESULT hr = DXGIGetDebugInterface1(0, __uuidof(IDXGIInfoQueue), &infoQueue);
	
	// 디버그 인터페이스 초기화 실패 시 (Release 모드 등)
	if (FAILED(hr))
	{
		// infoQueue가 nullptr로 유지되며, GetMessages에서 안전하게 처리됨
		messageBuffer[0] = '\0';
		return;
	}
	
	messageBuffer[0] = '\0';
}

ExceptionInfo& ExceptionInfo::GetCurrent()
{
	static ExceptionInfo instance;

	return instance;
}

void ExceptionInfo::Set() noexcept
{
	// infoQueue가 초기화되지 않았으면 종료
	if (!infoQueue)
		return;

	// 현재까지 저장된 메시지 수를 기록
	next = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	hasNewMessages = false;
}

const char* ExceptionInfo::GetSeverityString(DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity) const noexcept
{
	switch (severity)
	{
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
		return "[CORRUPTION]";

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
		return "[ERROR]";

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
		return "[WARNING]";

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
		return "[INFO]";

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
		return "[MESSAGE]";

	default:
		return "[UNKNOWN]";
	}
}

int ExceptionInfo::GetSeverityPriority(DXGI_INFO_QUEUE_MESSAGE_SEVERITY severity) const noexcept
{
	switch (severity)
	{
	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION:
		return 0; // 최우선

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR:
		return 1;

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING:
		return 2;

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO:
		return 3;

	case DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE:
		return 4;

	default:
		return 5;
	}
}

const char* ExceptionInfo::GetMessages()
{
	// infoQueue가 초기화되지 않았으면 빈 메시지 반환
	if (infoQueue == nullptr)
		return "[Error]\nNo DXGI Info Queue available.";

	const auto end = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	// 새로운 메시지가 없으면 캐시된 버퍼 반환
	if (next == end)
	{
		if (!hasNewMessages)
			return "";

		return messageBuffer;
	}

	// 이전에 읽은 메시지와 같으면 캐시 반환
	if (end == lastEnd && hasNewMessages)
		return messageBuffer;

	// 메시지 정보 수집
	size_t messageCount = 0;
	std::vector<std::unique_ptr<std::byte[]>> messageStorage; // 메시지 데이터 보관

	for (auto i = next; i < end; i++)
	{
		// DXGI가 전달할 수 있는 최대 메시지 수 도달 시 종료
		if (messageCount >= maxMessageCount)
			break;

		SIZE_T messageLength = 0;

		// i번째 메시지의 길이 가져오기
		HRESULT hr = infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
		Require::Check(hr, ErrorCode::CPP_NULLReference, "DXGI의 메시지 길이 조회에 실패했습니다.");

		// 메시지를 저장할 버퍼 생성
		auto bytes = make_unique<std::byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// i번째 메시지 가져오기
		hr = infoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
		Require::Check(hr, ErrorCode::CPP_OutOfRange, "DXGI의 메시지 조회에 실패했습니다.");

		// 메시지 정보 저장
		messageInfos[messageCount].severity = pMessage->Severity;
		messageInfos[messageCount].description = pMessage->pDescription;
		messageInfos[messageCount].length = strlen(pMessage->pDescription);

		messageStorage.push_back(std::move(bytes));
		messageCount++;
	}

	// 심각도 순으로 정렬 (ERROR → WARNING → INFO)
	std::sort(messageInfos, messageInfos + messageCount, 
		[this](const MessageInfo& a, const MessageInfo& b) {
			return GetSeverityPriority(a.severity) < GetSeverityPriority(b.severity);
		});

	// 버퍼 초기화
	size_t bufferPos = 0;
	messageBuffer[0] = '\0';

	// 정렬된 메시지를 버퍼에 복사
	for (size_t i = 0; i < messageCount; i++)
	{
		const auto& msgInfo = messageInfos[i];
		const char* severityStr = GetSeverityString(msgInfo.severity);
		const size_t severityLen = strlen(severityStr);
		const size_t requiredSpace = severityLen + 1 + msgInfo.length + 2; // "[LEVEL]\nmessage\n"

		// 버퍼 공간 확인 (마지막 null terminator 공간 확보)
		if (bufferPos + requiredSpace + 1 >= maxLogLength)
		{
			// 버퍼가 가득 찼으면 "..." 추가하고 종료
			const char* truncated = "... (truncated)\n";
			const size_t truncatedLen = strlen(truncated);
			
			if (bufferPos + truncatedLen + 1 < maxLogLength)
			{
				memcpy(messageBuffer + bufferPos, truncated, truncatedLen);
				bufferPos += truncatedLen;
			}

			break;
		}

		// 심각도 레벨 복사
		memcpy(messageBuffer + bufferPos, severityStr, severityLen);
		bufferPos += severityLen;
		messageBuffer[bufferPos] = '\n';
		bufferPos++;

		// 메시지 복사
		memcpy(messageBuffer + bufferPos, msgInfo.description, msgInfo.length);
		bufferPos += msgInfo.length;
		messageBuffer[bufferPos] = '\n';
		bufferPos++;

		// 메시지 구분을 위한 빈 줄 추가
		messageBuffer[bufferPos] = '\n';
		bufferPos++;
	}

	// Null terminator 추가
	messageBuffer[bufferPos] = '\0';

	// 캐시 상태 업데이트
	lastEnd = end;
	hasNewMessages = (bufferPos > 0);

	return messageBuffer;
}