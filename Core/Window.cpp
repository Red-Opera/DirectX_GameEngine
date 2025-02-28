#include "stdafx.h"
#include "Window.h"
#include "resource.h"

#include "External/Imgui/imgui_impl_win32.h"
using namespace std;

string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	// 에러 메세지를 반환하기 위한 문자열
	char* errorMessage = nullptr;

	// 입력한 에러 메세지에 대해서 일정한 형식에 맞춰 변역함
	const DWORD messageLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, 
		hr, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&errorMessage),
		0,
		nullptr
	);

	// 입력한 에러코드가 알수 없는 경우
	if (messageLen == 0)
		return "지정되지 않는 에러 코드";

	// 문자열을 string으로 변환 후 기존 문자열 동적 해제
	string errorString = errorMessage;
	LocalFree(errorMessage);

	return errorString;
}

Window::HRException::HRException(int line, const char* file, HRESULT hr) noexcept
	: Exception(line, file), hr(hr)
{

}

Window::HRException::HRException(int line, string file, HRESULT hr) noexcept
	: Exception(line, file), hr(hr)
{

}

const char* Window::HRException::what() const noexcept
{
	ostringstream out;

	out << "예외가 발생했습니다." << endl << endl << "예외 종류 : " <<
		string(GetType()) << "[0x" << uppercase << hex << hr <<"]" << endl << "예외 파일 : " <<
		GetFile() << endl << "줄 번호 : " + to_string(GetLine()) + "줄";

	exceptionText = out.str();
	return exceptionText.c_str();
}

const char* Window::HRException::GetType() const noexcept
{
	return "Window 창 생성 문제";
}

HRESULT Window::HRException::GetErrorCode() const noexcept
{
	return hr;
}

string Window::HRException::GetExptionContent() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

// =================================
//	Window Class
// =================================

Window::WindowClass Window::WindowClass::windowClass;

Window::WindowClass::WindowClass() noexcept
	: hInstance(GetModuleHandle(nullptr))	// 현재 실행 중인 Window창을 넘겨줌
{
	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WndMessageSetting,
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetInstance(),
	wcex.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0));
	wcex.hCursor = nullptr;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = GetName();
	wcex.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0));

	RegisterClassEx(&wcex);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClassA(windowClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return windowClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return windowClass.hInstance;
}

Window::Window(int width, int height, const char* name) : width(width), height(height)
{
	RECT rect;

	rect.left = 100;
	rect.right = rect.left + width;
	rect.top = 100;
	rect.bottom = rect.top + height;

	if (AdjustWindowRect(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
		throw LASTEXCEPT();

	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if (hWnd == nullptr)
		throw LASTEXCEPT();

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	ImGui_ImplWin32_Init(hWnd);
	graphic = make_unique<DxGraphic>(hWnd);

	RAWINPUTDEVICE rawDevice;
	rawDevice.usUsagePage = 0x01;
	rawDevice.usUsage = 0x02;
	rawDevice.dwFlags = 0;
	rawDevice.dwFlags = 0;
	rawDevice.hwndTarget = nullptr;

	if (RegisterRawInputDevices(&rawDevice, 1, sizeof(rawDevice)) == FALSE)
		throw Window::HRException(__LINE__, __FILE__, GetLastError());
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const string& title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
		throw LASTEXCEPT();
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;

	// 커서를 보여주고 커서가 마음대로 움직일 수 있게 설정
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;

	// 커서를 없애고 커서를 움직이지 못하게 설정
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::GetCursorEnabled() const noexcept
{
	return cursorEnabled;
}

optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// 사용자로부터 종료 메시지를 받은 경우
		if (msg.message == WM_QUIT)
			return (int)msg.wParam;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// 종료 메시지를 받지 않았을 경우 빈 메세지를 반환함
	return {};
}

DxGraphic& Window::GetDxGraphic()
{
	if (!graphic) 
	{
		throw NOGRAPHICEXEPT();
	}

	return *graphic;
}

void Window::ConfineCursor() noexcept
{
	RECT rect;

	GetClientRect(hWnd, &rect);											// 현재 클라이언트 영역을 가져옴
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);	// 클라이언트 영역 좌표를 윈도우 기준에서 화면 기준으로 변환 
	ClipCursor(&rect);													// rect에 저장된 클라이언트 영역안으로 제한
}

void Window::FreeCursor() noexcept
{
	// 커서 제한이 없도록 설정
	ClipCursor(nullptr);
}

void Window::ShowCursor() noexcept
{
	// 커서 감추기를 한 만큼 커서보이기를 함
	while (::ShowCursor(TRUE) < 0);
}

void Window::HideCursor() noexcept
{
	// 커서 보이기를 한 만큼 커서 감추기를 함
	while (::ShowCursor(FALSE) >= 0);
}

void Window::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT Window::WndMessageSetting(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// 창이 생성될 때 실행경우 (NC(Non-Client) : 타이틀 바, 테두리 등의 비클라이언트 영역을 의미)
	if (msg == WM_NCCREATE)
	{
		// lParam에 저장된 사용자 지정 설정을 CREATESTRUCTW로 변환하여 CreateWindow 함수 실행시 전달된 데이터를 가져옴
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		 
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));					// Window 창을 사용자 지정 설정대로 제작한다고 알린 후
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));  // Window 창 메세지도 HandleMsgThunk메소드가 처리한다는 것을 알림 (즉, 메세지가 있을 때 HandleMsgThunk 함수가 실행)

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// WndMessageSetting에서 설정한 GWLP_USERDATA에 따른 데이터를 가져롬
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// 해석한 메세지를 반환함
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	const auto imGuiIO = ImGui::GetIO();

	switch (msg)
	{
	// 창을 닫는 메세지가 들어온 경우
	case WM_CLOSE:
		PostQuitMessage(0);

	// Window 창에서 포커스를 잃을 경우
	case WM_KILLFOCUS:
		keyBoard.ClearState();
		break;

	// Window 창이 활성화 되었을 경우
	case WM_ACTIVATE:
		if (!cursorEnabled)
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			}

			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;

	// =================================
	//	키보드 메세지 관련 이벤트
	// =================================
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		// ImGui에서 캡션에 키보드로 입력하고 있을 때는 키보드 처리를 안함
		if (imGuiIO.WantCaptureKeyboard)
			break;

		// 이전에 키보드를 누른적이 없거나 지속적으로 눌렸을 때 키누르는 것을 인정할 경우
		if (!(lParam & 0x40000000) || keyBoard.AutorepeatIsEnabled())
			keyBoard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (imGuiIO.WantCaptureKeyboard)
			break;

		keyBoard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;

	case WM_CHAR:
		if (imGuiIO.WantCaptureKeyboard)
			break;

		keyBoard.OnChar(static_cast<unsigned char>(wParam));
		break;

	// =================================
	//	마우스 메세지 관련 이벤트
	// =================================
	case WM_MOUSEMOVE:
	{
		const POINTS point = MAKEPOINTS(lParam);

		if (!cursorEnabled)
		{
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
				HideCursor();
			}

			break;
		}

		// ImGui에 마우스를 이동시킬 때 처리 안함
		if (imGuiIO.WantCaptureMouse)
			break;

		// 마우스가 클라이언트 창 안에 있을 경우
		if (point.x >= 0 && point.y >= 0 && point.x < width && point.y < height)
		{
			// 마우스 위치를 저장함
			mouse.OnMouseMove(point.x, point.y);

			// 이전에 마우스가 창 안에 있지 않았을 경우 
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);		// 해당 이 Window 창을 캡처하여 해당 창이 마우스를 입력받고 다른 Window 창은 전달하지 않도록 함
				mouse.OnMouseEnter();
			}

			else
			{
				// 왼쪽 마우스 또는 오른쪽 마우스 버튼을 누를 경우
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
					mouse.OnMouseMove(point.x, point.y);

				// 마우스를 끌다가 창 밖에서 놓을 경우
				else
				{
					ReleaseCapture();
					mouse.OnMouseLeave();
				}
			}
		}

		break;
	}

	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow(hWnd);

		if (!cursorEnabled)
		{
			ConfineCursor();
			HideCursor();
		}

		if (imGuiIO.WantCaptureMouse)
			break;

		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(point.x, point.y);

		break;
	}

	case WM_RBUTTONDOWN:
	{
		if (imGuiIO.WantCaptureMouse)
			break;

		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnRightPressed(point.x, point.y);

		break;
	}

	case WM_LBUTTONUP:
	{
		if (imGuiIO.WantCaptureMouse)
			break;

		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(point.x, point.y);

		if (point.x < 0 || point.x >= WINWIDTH || point.y < 0 || point.y >= WINHEIGHT)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}

		break;
	}

	case WM_RBUTTONUP:
	{
		if (imGuiIO.WantCaptureMouse)
			break;

		const POINTS point = MAKEPOINTS(lParam);
		mouse.OnRightReleased(point.x, point.y);

		if (point.x < 0 || point.x >= WINWIDTH || point.y < 0 || point.y >= WINHEIGHT)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}

		break;
	}

	case WM_MOUSEWHEEL:
	{
		if (imGuiIO.WantCaptureMouse)
			break;

		const POINTS point = MAKEPOINTS(lParam);
		
		// 마우스 휠을 올릴 경우
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			mouse.OnWheelUp(point.x, point.y);

		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			mouse.OnWheelDown(point.x, point.y);

		break;
	}

	case WM_INPUT:
	{
		if (!mouse.RawEnabled())
			break;

		UINT size;
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
			break;
		
		rawBuffer.resize(size);

		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) == -1)
			break;

		auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());

		if (rawInput.header.dwType == RIM_TYPEMOUSE && (rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0))
			mouse.OnRawDelta(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);

		break;
	}

		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

const char* Window::NoGraphicException::GetType() const noexcept
{
	return "Window 창 생성 예외 (그래픽 클래스가 없음)";
}
