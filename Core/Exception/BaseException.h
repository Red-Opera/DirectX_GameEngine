#pragma once

#include <exception>
#include <string>

// 예외처리를 위한 기본적인 클래스
class BaseException : public std::exception
{
public:
	BaseException(int line, const char* file) noexcept;
	BaseException(int line, std::string file) noexcept;

	const char* what() const noexcept override;				// 어떤 예외가 있는지 알려주는 메소드

	virtual const char* GetType() const noexcept;			// 어떤 예외 종류인지 알려주는 메소드
	int GetLine() const noexcept;							// 어떤 줄에 예외가 발생했는지 알려주는 함수
	const std::string& GetFile() const noexcept;			// 어떤 코드 파일에서 발생했는지 알려주는 메소드
	std::string GetFileNameAndLine() const noexcept;				// 예외가 발생한 파일 이름과 줄 번호를 알려주는 메소드

protected:
	mutable std::string exceptionText;	// 예외가 발생했을 때 예외 내용을 받을 변수

private:
	int line;		// 예외가 생긴 줄 번호
	std::string file;	// 예외가 발생한 파일 이름
};