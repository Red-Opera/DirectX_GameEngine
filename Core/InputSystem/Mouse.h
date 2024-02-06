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
		// ���콺 �̺�Ʈ
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

		// �ش� ���콺 �̺�Ʈ ����
		Type GetType() const noexcept { return type; }

		// ���콺 �̺�Ʈ�� �߻��� ���콺 ������ ��ġ�� ��� �޼ҵ�
		pair<int, int> GetPos() const noexcept { return{ x,y }; }
		int GetPosX() const noexcept { return x; }
		int GetPosY() const noexcept { return y; }

		// �ش� ���콺�� ���ȴ��� Ȯ���ϴ� �޼ҵ�
		bool LeftIsPressed() const noexcept { return leftIsPressed; }
		bool RightIsPressed() const noexcept { return rightIsPressed; }

	private:
		Type type;				// �ش� ���콺 �̺�Ʈ Ÿ��
		bool leftIsPressed;		// ���� Ű�� ���ȴ��� ����
		bool rightIsPressed;	// ������ Ű�� ���ȴ��� ����
		int x;					// ���콺 x��ǥ
		int y;					// ���콺 y��ǥ
	};

public:
	Mouse() = default;

	// ���� ���� ����
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

	int x;	// ���콺 x��ǥ
	int y;	// ���콺 y��ǥ

	bool leftIsPressed = false;		// ���� Ű�� ���ȴ��� ����
	bool rightIsPressed = false;	// ������ Ű�� ���ȴ��� ����

	bool isInWindow = false;		// ���� Window â �ȿ� �ִ��� ����
	int wheelDeltaCarry = 0;		// ���� ���콺 �� �ӵ�
	bool rawEnabled = false;

	std::queue<Event> buffer;
	std::queue<RawDelta> rawDeltaBuffer;
};