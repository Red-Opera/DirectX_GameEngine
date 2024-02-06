#include "stdafx.h"
#include "ExceptionInfo.h"

#include "../Window.h"
#include "../DxGraphic.h"

#include <memory>
#include <dxgidebug.h>

#pragma comment(lib, "dxguid")

#define INFOEXCEPTION(hr) if (FAILED(hr)) throw DxGraphic::HRException(__LINE__, __FILE__, hr)

ExceptionInfo::ExceptionInfo()
{
	// dxgidebug.dll 라이브러리 안에 있는 DXGIGetDebugInterface 함수를 함수명으로 표시하기 위한 정의
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// dxgidebug.dll 라이브러리 파일을 가져옴
	const auto modDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	// 라이브러리를 성공적으로 가져왔는지 확인
	if (modDxgiDebug == nullptr)
	{
		throw LASTEXCEPT();
	}

	// dxgidebug.dll 라이브러리 파일 안에 있는 DXGIGetDebugInterface의 주소를 가져와 DXGIGetDebugInterface로 변환함
	const auto dxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(modDxgiDebug, "DXGIGetDebugInterface")));

	// DXGIGetDebugInterface 프로시저를 성공적으로 가져왔는지 확인
	if (dxgiGetDebugInterface == nullptr)
	{
		throw LASTEXCEPT();
	}

	// DXGIGetDebugInterface라는 프로시저를 실행하여 infoQueue를 가져옴
	INFOEXCEPTION(dxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &infoQueue));
}

void ExceptionInfo::Set() noexcept
{
	next = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

vector<string> ExceptionInfo::GetMessages() const
{
	vector<string> messages;

	const auto end = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for (auto i = next; i < end; i++)
	{
		SIZE_T messageLength = 0;

		// i번째에 해당하는 디버그 메세지의 길이를 messageLength에 반환함
		INFOEXCEPTION(infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		// 디버그 메세지를 얻기 위한 버퍼 생성
		auto bytes = make_unique<std::byte[]>(messageLength);
		auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// i번쨰에 해당하는 디버그 메세지를 가져옴
		INFOEXCEPTION(infoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength));
		messages.emplace_back(message->pDescription);
	}

	// 디버그 메세지를 반환함
	return messages;
}
