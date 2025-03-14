#include "stdafx.h"
#include "Window.h"
#include "App.h"

LPSTR ConvertPWSTRToLPSTR(PWSTR pwsz)
{
	// 유니코드 문자열의 길이를 구함 (null-terminator 포함)
	int lenW = lstrlenW(pwsz) + 1;

	// 멀티바이트로 변환했을 때의 길이를 구함
	int lenA = WideCharToMultiByte(CP_ACP, 0, pwsz, lenW, NULL, 0, NULL, NULL);

	// 변환된 문자열을 저장할 메모리 할당
	LPSTR pszA = new char[lenA];

	// 실제 변환 수행
	WideCharToMultiByte(CP_ACP, 0, pwsz, lenW, pszA, lenA, NULL, NULL);

	return pszA;
}

int CALLBACK wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	PWSTR pCmdLine, 
	int nCmdShow)
{
	try
	{
		// PWSTR -> LPSTR 변환
		LPSTR cmdLineA = ConvertPWSTRToLPSTR(pCmdLine);

		return App{ cmdLineA }.Run();
	}
	
	catch (const BaseException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}

	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "표준 예외", MB_OK | MB_ICONEXCLAMATION);
	}

	catch (...)
	{
		MessageBox(nullptr, "세부적인 내용을 알 수 없음", "알 수 없는 예외", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}