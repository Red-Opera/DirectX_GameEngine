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
		// 키보드 입력 상태
		enum class Type { Press, Release, };
	
	public:
		Event(Type type, unsigned char code) noexcept : type(type), code(code) {}

		bool IsPress() const noexcept { return type == Type::Press; }		// 키보드의 해당 키가 눌렸는지 여부
		bool IsRelease() const noexcept { return type == Type::Release; }	// 키보드의 해당 키를 떼고 있는지 여부
		unsigned char GetCode() const noexcept { return code; }

	private:
		Type type;			// 키보드 이벤트 중 타입
		unsigned char code;	// 해당 이벤트의 키보드 값
	};

public:
	Keyboard() = default;

	// 복사 대입 금지
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// 키 이벤트 관련 메소드
	bool IsPressed(unsigned char keycode) const noexcept;	// 현재 해당 키가 눌렸는지 여부
	std::optional<Event> ReadKey() noexcept;				// 키보드 이벤트 중 하나를 가져오는 메소드
	bool KeyIsEmpty() const noexcept;						// 키 이벤트가 존재하는지 여부
	void FlushKey() noexcept;								// 키 이벤트를 모두 제거함

	// 키보드로 입력한 문자관련 이벤트
	std::optional<char> ReadChar() noexcept;	// 해당 문자
	bool CharIsEmpty() const noexcept;			// 키보드로 입력된 문자가 없는지 여부
	void FlushChar() noexcept;					// 키보드로 입력된 문자를 모두 제거
	void Flush() noexcept;						// 이벤트 및 문자 모두 제거

	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;	// 지속적으로 누를 때 KeyDown이라고 인정하는지 여부

private:
	void OnKeyPressed(unsigned char keycode) noexcept;		// 키보드가 눌렸을 때 처리하는 메소드
	void OnKeyReleased(unsigned char keycode) noexcept;		// 키보드에서 뗐을 때 처리하는 메소드
	void OnChar(char character) noexcept;					// 키보드에서 문자를 입력했을 때 처리하는 메소드
	void ClearState() noexcept;

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;	// 버퍼보다 이벤트 양이 많을 경우 맨 앞에 있는 버퍼 제거


	static constexpr unsigned int nKeys = 256u;			// 키보드 키의 개수
	static constexpr unsigned int bufferSize = 16u;		// 이벤트 버퍼 개수

	bool autorepeatEnabled = false;	// 지속적으로 누를 때 KeyDown이라고 인정하는지 여부

	std::bitset<nKeys> keystates;	// 각 키별로 상태를 저장하는 값
	std::queue<Event> keybuffer;	// 키 이벤트를 저장하는 값
	std::queue<char> charbuffer;	// 해당 문자를 입력했는지 확인하는 값
};