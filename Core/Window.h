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
		
		// 예외 정보에 관한 메소드
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

	// 커서 관련 메소드
	void EnableCursor() noexcept;				// 커서를 사용할 수 있도록 설정하는 메소드
	void DisableCursor() noexcept;				// 커서를 사용할 수 없도록 설정하는 메소드
	bool GetCursorEnabled() const noexcept;		// 커서 사용할 수 있는 여부를 반환하는 메소드

	static std::optional<int> ProcessMessages() noexcept;
	static void ShowGameFrame(HWND hWnd) noexcept;

	int GetClientWidht() { return width; }			// 클라이언트 창 너비
	int GetClientHeight() { return height; }		// 클라이언트 창 높이

	static void SetDxGraphic(std::unique_ptr<DxGraphic> graphic);	// 그래픽 관련 클래스를 설정하는 메소드
	static DxGraphic& GetDxGraphic();								// 그래픽 관련 클래스를 얻는 메소드
	HWND GetHWnd() const { return hWnd; }

	// =================================
	//	Class Parameter
	// =================================

	Keyboard keyBoard;	// 키보드 클래스
	Mouse mouse;		// 마우스 클래스

private:
	// =================================
	//	Inner Class
	// =================================

	// Window 창은 한개이기 때문에 싱글톤으로 작성
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();

		// 복사 대입 금지
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		static constexpr const char* windowClassName = "DirectX 3D Game";
		static WindowClass windowClass;
		
		HINSTANCE hInstance;
	};

	// =================================
	//	Class Method
	// =================================

	// 커서 관련 메소드
	void ConfineCursor() noexcept;			// 커서를 특정 영역안에 제한하는 메소드
	void FreeCursor() noexcept;				// 커서 이동제한이 없도록 설정하는 메소드
	void ShowCursor() noexcept;				// 커서를 보이기 위한 메소드
	void HideCursor() noexcept;				// 커서를 감추기 위한 메소드
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	
	static LRESULT CALLBACK WndMessageSetting(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	// =================================
	//	Class Parameter
	// =================================

	bool cursorEnabled = true;			// 커서가 보이는 여부
	
	int width;							// 클라이언트 창 너비
	int height;							// 클라이언트 창 높이

	HWND hWnd;

	static std::unique_ptr<DxGraphic> graphic; // DierctX 3D 그래픽 클래스

	std::vector<BYTE> rawBuffer;
	std::string commandLine;


	static std::string currentSceneName;
};