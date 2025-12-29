#pragma once

#include <array>

enum class ErrorCode : unsigned int
{
	None = 0,

	// C++ 관련 오류
	CPP_OutOfRange,
	CPP_NULLReference,
	CPP_DivisionByZero,

	// WinAPI 관련 오류
	WINAPI_WindowCreateFailed,
	WINAPI_DxGraphicInvalid,
	WINAPI_HandleInvalid,

	// 그래픽스 관련 오류
	GRAPHICS_DeviceInvalid,
	GRAPHICS_BufferCreateFailed,
	GRAPHICS_GetBufferFailed,
	GRAPHICS_BindFailed,
	GRAPHICS_ShaderCompileFailed,
	GRAPHICS_ShaderLoadSaveFailed,
	GRAPHICS_TextureLoadFailed,
	GRAPHICS_MapUnmapFailed,
	GRAPHICS_ETC,

	// 렌더 그래프 관련 오류
	RENDERGRAPH_RenderTargetInvalid,
	RENDERGRAPH_PipelineStateInvalid,
	RENDERGRAPH_DataConsumerInvalid,
	RENDERGRAPH_DataConsumerNameInvalid,
	RENDERGRAPH_DataTypeMismatch,
	RENDERGRAPH_ETC,

	Count
};

constexpr std::array<const char*, static_cast<size_t>(ErrorCode::Count)> errorCodeStrings =
{
	"정상",

	// C++ 관련 오류
	"배열 범위 초과",
	"널 포인터 접근",
	"나눗셈 연산 오류",

	// WinAPI 관련 오류
	"Window API 창 생성 실패",
	"그래픽스 API 접근 변수 유효하지 않음",
	"창 접근 변수 오류",

	// 그래픽스 관련 오류
	"그래픽스 Device가 유효하지 않음",
	"그래픽스 버퍼 생성 실패",
	"그래픽스 버퍼 가져오기 실패",
	"그래픽스 데이터 파이프라인 결합 실패",
	"그래픽스 셰이더 컴파일 실패",
	"그래픽스 셰이더 불러오기 실패",
	"그래픽스 텍스처 로딩 실패",
	"그래픽스 맵/언맵 실패",
	"그래픽스 기타 오류",

	// 렌더 그래프 관련 오류
	"렌더 그래프 렌더 타켓이 유효하지 않음",
	"렌더 그래프 파이프라인 상태가 유효하지 않음",
	"렌더 그래프 데이터 소비자가 존재하지 않음",
	"렌더 그래프 데이터 소비자의 이름이 잘못됨",
	"렌더 그래프 데이터 타입이 일치하지 않음",
	"렌더 그래프 기타 오류"
};

static_assert(errorCodeStrings.size() == static_cast<size_t>(ErrorCode::Count), "ErrorCodeStrings 배열 크기가 ErrorCode 열거형의 크기와 일치하지 않습니다.");