#pragma once
#include <queue>
#include <optional>
using namespace std;

class Mouse
{
	friend class Window;

public:
	struct RawDelta { int x, y; };

	class Event
	{
	public:
		// 마우스 이벤트
		enum class Type
		{
			LPress, LRelease,
			RPress, RRelease,
			WheelUp, WheelDown,
			Move, Enter, Leave,
		};

	public:
		Event(Type type, const Mouse& parent) noexcept
			: type(type),
			leftIsPressed(parent.leftIsPressed), rightIsPressed(parent.rightIsPressed),
			x(parent.x), y(parent.y) {}

		// 해당 마우스 이벤트 종류
		Type GetType() const noexcept { return type; }

		// 마우스 이벤트가 발생한 마우스 포인터 위치를 얻는 메소드
		pair<int, int> GetPos() const noexcept { return{ x,y }; }
		int GetPosX() const noexcept { return x; }
		int GetPosY() const noexcept { return y; }

		// 해당 마우스가 눌렸는지 확인하는 메소드
		bool LeftIsPressed() const noexcept { return leftIsPressed; }
		bool RightIsPressed() const noexcept { return rightIsPressed; }

	private:
		Type type;				// 해당 마우스 이벤트 타입
		bool leftIsPressed;		// 왼쪽 키가 눌렸는지 여부
		bool rightIsPressed;	// 오른쪽 키가 눌렸는지 여부
		int x;					// 마우스 x좌표
		int y;					// 마우스 y좌표
	};

public:
	Mouse() = default;

	// 복사 생성 제거
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	std::optional<RawDelta> ReadRawDelta() noexcept;

	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;

	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;

	optional<Mouse::Event> Read() noexcept;
	bool IsEmpty() const noexcept { return buffer.empty(); }
	void Flush() noexcept;

	void EnableRaw() noexcept;
	void DisableRaw() noexcept;
	bool RawEnabled() const noexcept;

private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;

	void OnRawDelta(int dx, int dy) noexcept;

	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;

	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;

	void TrimBuffer() noexcept;
	void TrimRawInputBuffer() noexcept;

private:
	static constexpr unsigned int bufferSize = 16u;

	int x;	// 마우스 x좌표
	int y;	// 마우스 y좌표

	bool leftIsPressed = false;		// 왼쪽 키가 눌렸는지 여부
	bool rightIsPressed = false;	// 오른쪽 키가 눌렸는지 여부

	bool isInWindow = false;		// 현재 Window 창 안에 있는지 여부
	int wheelDeltaCarry = 0;		// 현재 마우스 휠 속도
	bool rawEnabled = false;

	std::queue<Event> buffer;
	std::queue<RawDelta> rawDeltaBuffer;
};