#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Window;

public:
	class Event
	{
	public:
		// Ű���� �Է� ����
		enum class Type { Press, Release, };
	
	public:
		Event(Type type, unsigned char code) noexcept : type(type), code(code) {}

		bool IsPress() const noexcept { return type == Type::Press; }		// Ű������ �ش� Ű�� ���ȴ��� ����
		bool IsRelease() const noexcept { return type == Type::Release; }	// Ű������ �ش� Ű�� ���� �ִ��� ����
		unsigned char GetCode() const noexcept { return code; }

	private:
		Type type;			// Ű���� �̺�Ʈ �� Ÿ��
		unsigned char code;	// �ش� �̺�Ʈ�� Ű���� ��
	};

public:
	Keyboard() = default;

	// ���� ���� ����
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// Ű �̺�Ʈ ���� �޼ҵ�
	bool IsPressed(unsigned char keycode) const noexcept;	// ���� �ش� Ű�� ���ȴ��� ����
	std::optional<Event> ReadKey() noexcept;				// Ű���� �̺�Ʈ �� �ϳ��� �������� �޼ҵ�
	bool KeyIsEmpty() const noexcept;						// Ű �̺�Ʈ�� �����ϴ��� ����
	void FlushKey() noexcept;								// Ű �̺�Ʈ�� ��� ������

	// Ű����� �Է��� ���ڰ��� �̺�Ʈ
	std::optional<char> ReadChar() noexcept;	// �ش� ����
	bool CharIsEmpty() const noexcept;			// Ű����� �Էµ� ���ڰ� ������ ����
	void FlushChar() noexcept;					// Ű����� �Էµ� ���ڸ� ��� ����
	void Flush() noexcept;						// �̺�Ʈ �� ���� ��� ����

	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;	// ���������� ���� �� KeyDown�̶�� �����ϴ��� ����

private:
	void OnKeyPressed(unsigned char keycode) noexcept;		// Ű���尡 ������ �� ó���ϴ� �޼ҵ�
	void OnKeyReleased(unsigned char keycode) noexcept;		// Ű���忡�� ���� �� ó���ϴ� �޼ҵ�
	void OnChar(char character) noexcept;					// Ű���忡�� ���ڸ� �Է����� �� ó���ϴ� �޼ҵ�
	void ClearState() noexcept;

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;	// ���ۺ��� �̺�Ʈ ���� ���� ��� �� �տ� �ִ� ���� ����


	static constexpr unsigned int nKeys = 256u;			// Ű���� Ű�� ����
	static constexpr unsigned int bufferSize = 16u;		// �̺�Ʈ ���� ����

	bool autorepeatEnabled = false;	// ���������� ���� �� KeyDown�̶�� �����ϴ��� ����

	std::bitset<nKeys> keystates;	// �� Ű���� ���¸� �����ϴ� ��
	std::queue<Event> keybuffer;	// Ű �̺�Ʈ�� �����ϴ� ��
	std::queue<char> charbuffer;	// �ش� ���ڸ� �Է��ߴ��� Ȯ���ϴ� ��
};