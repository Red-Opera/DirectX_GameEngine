#pragma once

#include "stdafx.h"
#include "DxGraphic.h"

#include "Exception/BaseException.h"
#include "Exception/WindowException.h"

#include "InputSystem/KeyBoard.h"
#include "InputSystem/Mouse.h"

#include <optional>
#include <memory>

class Window
{
public:
	// =================================
	//	Inner Class
	// =================================

	class Exception : public BaseException
	{
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;

	private:
		using BaseException::BaseException;
	};

	class HRException : public Exception
	{
	public:
		HRException(int line, const char* file, HRESULT hr) noexcept;
		HRException(int line, std::string file, HRESULT hr) noexcept;

		const char* what() const noexcept override;
		
		// ���� ������ ���� �޼ҵ�
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetExptionContent() const noexcept;

	private:
		HRESULT hr;
	};

	class NoGraphicException : public BaseException
	{
	public: 
		BaseException::BaseException;
		const char* GetType() const noexcept override;
	};

	// =================================
	//	Class Method
	// =================================

	Window(int width, int height, const char* name);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	static void SetTitle(HWND hWnd, const std::string title);

	// Ŀ�� ���� �޼ҵ�
	void EnableCursor() noexcept;				// Ŀ���� ����� �� �ֵ��� �����ϴ� �޼ҵ�
	void DisableCursor() noexcept;				// Ŀ���� ����� �� ������ �����ϴ� �޼ҵ�
	bool GetCursorEnabled() const noexcept;		// Ŀ�� ����� �� �ִ� ���θ� ��ȯ�ϴ� �޼ҵ�

	static std::optional<int> ProcessMessages() noexcept;
	static void ShowGameFrame(HWND hWnd) noexcept;

	int GetClientWidht() { return width; }			// Ŭ���̾�Ʈ â �ʺ�
	int GetClientHeight() { return height; }		// Ŭ���̾�Ʈ â ����

	static void SetDxGraphic(std::unique_ptr<DxGraphic> graphic);	// �׷��� ���� Ŭ������ �����ϴ� �޼ҵ�
	static DxGraphic& GetDxGraphic();								// �׷��� ���� Ŭ������ ��� �޼ҵ�
	HWND GetHWnd() const { return hWnd; }

	// =================================
	//	Class Parameter
	// =================================

	Keyboard keyBoard;	// Ű���� Ŭ����
	Mouse mouse;		// ���콺 Ŭ����

private:
	// =================================
	//	Inner Class
	// =================================

	// Window â�� �Ѱ��̱� ������ �̱������� �ۼ�
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();

		// ���� ���� ����
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		static constexpr const char* windowClassName = "DirectX 3D Game";
		static WindowClass windowClass;
		
		HINSTANCE hInstance;
	};

	// =================================
	//	Class Method
	// =================================

	// Ŀ�� ���� �޼ҵ�
	void ConfineCursor() noexcept;			// Ŀ���� Ư�� �����ȿ� �����ϴ� �޼ҵ�
	void FreeCursor() noexcept;				// Ŀ�� �̵������� ������ �����ϴ� �޼ҵ�
	void ShowCursor() noexcept;				// Ŀ���� ���̱� ���� �޼ҵ�
	void HideCursor() noexcept;				// Ŀ���� ���߱� ���� �޼ҵ�
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	
	static LRESULT CALLBACK WndMessageSetting(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	// =================================
	//	Class Parameter
	// =================================

	bool cursorEnabled = true;			// Ŀ���� ���̴� ����
	
	int width;							// Ŭ���̾�Ʈ â �ʺ�
	int height;							// Ŭ���̾�Ʈ â ����

	HWND hWnd;

	static std::unique_ptr<DxGraphic> graphic; // DierctX 3D �׷��� Ŭ����

	std::vector<BYTE> rawBuffer;
	std::string commandLine;


	static std::string currentSceneName;
};