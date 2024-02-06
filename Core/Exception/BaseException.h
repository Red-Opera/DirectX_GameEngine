#pragma once

using namespace std;

// ����ó���� ���� �⺻���� Ŭ����
class BaseException : public exception
{
public:
	BaseException(int line, const char* file) noexcept;
	BaseException(int line, string file) noexcept;

	const char* what() const noexcept override;				// � ���ܰ� �ִ��� �˷��ִ� �޼ҵ�

	virtual const char* GetType() const noexcept;			// � ���� �������� �˷��ִ� �޼ҵ�
	int GetLine() const noexcept;							// � �ٿ� ���ܰ� �߻��ߴ��� �˷��ִ� �Լ�
	const std::string& GetFile() const noexcept;			// � �ڵ� ���Ͽ��� �߻��ߴ��� �˷��ִ� �޼ҵ�
	string GetFileNameAndLine() const noexcept;				// ���ܰ� �߻��� ���� �̸��� �� ��ȣ�� �˷��ִ� �޼ҵ�

protected:
	mutable string exceptionText;	// ���ܰ� �߻����� �� ���� ������ ���� ����

private:
	int line;		// ���ܰ� ���� �� ��ȣ
	string file;	// ���ܰ� �߻��� ���� �̸�
};