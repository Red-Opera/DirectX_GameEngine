// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#define _USE_MATH_DEFINES
#include <cmath>

// Windows 헤더 파일
#include <windowsx.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <assert.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include "Utility/GameTimer.h"
#include "Utility/MathInfo.h"
#include "Utility/Vector.h"

#include "Utility/Imgui/imgui.h"

#include <DX3DApp.h>

// Define
#define HR(x) { HRESULT hr = x; if(FAILED(hr)) { assert(false); } }
#define ReleaseCOM(x) { if(x) { (x)->Release(); (x) = nullptr; } }
#define WINWIDTH 1280
#define WINHEIGHT 720

// namespace
namespace DxMath = DirectX;