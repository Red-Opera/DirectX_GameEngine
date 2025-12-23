#include "stdafx.h"
#include "Exception.h"

void Exception::Error(const ErrorCode&& errorCode, const char* message)
{
	// 에러 정보 출력
	std::string errorCodeStr = errorCodeStrings[static_cast<size_t>(errorCode)];

	// 디버그 출력
	std::string result = "========== Exception::Error 호출됨 ==========\n";
	result += "에러 코드 : " + errorCodeStr + "\n";

	if (message)
		result += "메시지 : " + std::string(message) + "\n";

	result += "=============================================\n";

	// assert false로 디버거 중단
	assert(false && result.c_str());
}
