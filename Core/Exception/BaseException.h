#pragma once

#include <exception>
#include <string>

// ����ó���� ���� �⺻���� Ŭ����
class BaseException : public std::exception
{
public:
	BaseException(int line, const char* file) noexcept;
	BaseException(int line, std::string file) noexcept;

	const char* what() const noexcept override;				// � ���ܰ� �ִ��� �˷��ִ� �޼ҵ�

	virtual const char* GetType() const noexcept;			// � ���� �������� �˷��ִ� �޼ҵ�
	int GetLine() const noexcept;							// � �ٿ� ���ܰ� �߻��ߴ��� �˷��ִ� �Լ�
	const std::string& GetFile() const noexcept;			// � �ڵ� ���Ͽ��� �߻��ߴ��� �˷��ִ� �޼ҵ�
	std::string GetFileNameAndLine() const noexcept;				// ���ܰ� �߻��� ���� �̸��� �� ��ȣ�� �˷��ִ� �޼ҵ�

protected:
	mutable std::string exceptionText;	// ���ܰ� �߻����� �� ���� ������ ���� ����

private:
	int line;		// ���ܰ� ���� �� ��ȣ
	std::string file;	// ���ܰ� �߻��� ���� �̸�
};