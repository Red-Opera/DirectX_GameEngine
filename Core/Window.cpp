#include "stdafx.h"
#include "Window.h"
#include "resource.h"

#include "External/Imgui/imgui_impl_win32.h"
using namespace std;

string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	// ���� �޼����� ��ȯ�ϱ� ���� ���ڿ�
	char* errorMessage = nullptr;

	// �Է��� ���� �޼����� ���ؼ� ������ ���Ŀ� ���� ������
	const DWORD messageLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, 
		hr, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&errorMessage),
		0,
		nullptr
	);

	// �Է��� �����ڵ尡 �˼� ���� ���
	if (messageLen == 0)
		return "�������� �ʴ� ���� �ڵ�";

	// ���ڿ��� string���� ��ȯ �� ���� ���ڿ� ���� ����
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

	out << "���ܰ� �߻��߽��ϴ�." << endl << endl << "���� ���� : " <<
		string(GetType()) << "[0x" << uppercase << hex << hr <<"]" << endl << "���� ���� : " <<
		GetFile() << endl << "�� ��ȣ : " + to_string(GetLine()) + "��";

	exceptionText = out.str();
	return exceptionText.c_str();
}

const char* Window::HRException::GetType() const noexcept
{
	return "Window â ���� ����";
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
	: hInstance(GetModuleHandle(nullptr))	// ���� ���� ���� Windowâ�� �Ѱ���
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

	// Ŀ���� �����ְ� Ŀ���� ������� ������ �� �ְ� ����
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;

	// Ŀ���� ���ְ� Ŀ���� �������� ���ϰ� ����
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
		// ����ڷκ��� ���� �޽����� ���� ���
		if (msg.message == WM_QUIT)
			return (int)msg.wParam;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// ���� �޽����� ���� �ʾ��� ��� �� �޼����� ��ȯ��
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

	GetClientRect(hWnd, &rect);											// ���� Ŭ���̾�Ʈ ������ ������
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);	// Ŭ���̾�Ʈ ���� ��ǥ�� ������ ���ؿ��� ȭ�� �������� ��ȯ 
	ClipCursor(&rect);													// rect�� ����� Ŭ���̾�Ʈ ���������� ����
}

void Window::FreeCursor() noexcept
{
	// Ŀ�� ������ ������ ����
	ClipCursor(nullptr);
}

void Window::ShowCursor() noexcept
{
	// Ŀ�� ���߱⸦ �� ��ŭ Ŀ�����̱⸦ ��
	while (::ShowCursor(TRUE) < 0);
}

void Window::HideCursor() noexcept
{
	// Ŀ�� ���̱⸦ �� ��ŭ Ŀ�� ���߱⸦ ��
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
	// â�� ������ �� ������ (NC(Non-Client) : Ÿ��Ʋ ��, �׵θ� ���� ��Ŭ���̾�Ʈ ������ �ǹ�)
	if (msg == WM_NCCREATE)
	{
		// lParam�� ����� ����� ���� ������ CREATESTRUCTW�� ��ȯ�Ͽ� CreateWindow �Լ� ����� ���޵� �����͸� ������
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		 
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));					// Window â�� ����� ���� ������� �����Ѵٰ� �˸� ��
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));  // Window â �޼����� HandleMsgThunk�޼ҵ尡 ó���Ѵٴ� ���� �˸� (��, �޼����� ���� �� HandleMsgThunk �Լ��� ����)

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// WndMessageSetting���� ������ GWLP_USERDATA�� ���� �����͸� ������
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	// �ؼ��� �޼����� ��ȯ��
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	const auto imGuiIO = ImGui::GetIO();

	switch (msg)
	{
	// â�� �ݴ� �޼����� ���� ���
	case WM_CLOSE:
		PostQuitMessage(0);

	// Window â���� ��Ŀ���� ���� ���
	case WM_KILLFOCUS:
		keyBoard.ClearState();
		break;

	// Window â�� Ȱ��ȭ �Ǿ��� ���
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
	//	Ű���� �޼��� ���� �̺�Ʈ
	// =================================
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		// ImGui���� ĸ�ǿ� Ű����� �Է��ϰ� ���� ���� Ű���� ó���� ����
		if (imGuiIO.WantCaptureKeyboard)
			break;

		// ������ Ű���带 �������� ���ų� ���������� ������ �� Ű������ ���� ������ ���
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
	//	���콺 �޼��� ���� �̺�Ʈ
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

		// ImGui�� ���콺�� �̵���ų �� ó�� ����
		if (imGuiIO.WantCaptureMouse)
			break;

		// ���콺�� Ŭ���̾�Ʈ â �ȿ� ���� ���
		if (point.x >= 0 && point.y >= 0 && point.x < width && point.y < height)
		{
			// ���콺 ��ġ�� ������
			mouse.OnMouseMove(point.x, point.y);

			// ������ ���콺�� â �ȿ� ���� �ʾ��� ��� 
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);		// �ش� �� Window â�� ĸó�Ͽ� �ش� â�� ���콺�� �Է¹ް� �ٸ� Window â�� �������� �ʵ��� ��
				mouse.OnMouseEnter();
			}

			else
			{
				// ���� ���콺 �Ǵ� ������ ���콺 ��ư�� ���� ���
				if (wParam & (MK_LBUTTON | MK_RBUTTON))
					mouse.OnMouseMove(point.x, point.y);

				// ���콺�� ���ٰ� â �ۿ��� ���� ���
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
		
		// ���콺 ���� �ø� ���
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
	return "Window â ���� ���� (�׷��� Ŭ������ ����)";
}
