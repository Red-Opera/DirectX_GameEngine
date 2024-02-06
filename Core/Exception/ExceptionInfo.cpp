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
	// dxgidebug.dll ���̺귯�� �ȿ� �ִ� DXGIGetDebugInterface �Լ��� �Լ������� ǥ���ϱ� ���� ����
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// dxgidebug.dll ���̺귯�� ������ ������
	const auto modDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

	// ���̺귯���� ���������� �����Դ��� Ȯ��
	if (modDxgiDebug == nullptr)
	{
		throw LASTEXCEPT();
	}

	// dxgidebug.dll ���̺귯�� ���� �ȿ� �ִ� DXGIGetDebugInterface�� �ּҸ� ������ DXGIGetDebugInterface�� ��ȯ��
	const auto dxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(modDxgiDebug, "DXGIGetDebugInterface")));

	// DXGIGetDebugInterface ���ν����� ���������� �����Դ��� Ȯ��
	if (dxgiGetDebugInterface == nullptr)
	{
		throw LASTEXCEPT();
	}

	// DXGIGetDebugInterface��� ���ν����� �����Ͽ� infoQueue�� ������
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

		// i��°�� �ش��ϴ� ����� �޼����� ���̸� messageLength�� ��ȯ��
		INFOEXCEPTION(infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		// ����� �޼����� ��� ���� ���� ����
		auto bytes = make_unique<std::byte[]>(messageLength);
		auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// i������ �ش��ϴ� ����� �޼����� ������
		INFOEXCEPTION(infoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength));
		messages.emplace_back(message->pDescription);
	}

	// ����� �޼����� ��ȯ��
	return messages;
}
