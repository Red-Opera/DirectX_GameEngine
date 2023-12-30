// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include "BaseFile/targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#include <windows.h>
#include <windowsx.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3d11.h>
#include <assert.h>

#include <iostream>
#include <sstream>

#include <Utility/GameTimer.h>
#include <Utility/d3dUtil.h>

#include <DX3DApp.h>

// Define
#define HR(x) { HRESULT hr = x; if(FAILED(hr)) { assert(false); } }
#define ReleaseCOM(x) { if(x) { (x)->Release(); (x) = nullptr; } }