#include "stdafx.h"

// ========================================================
//	재질의 설정
// ========================================================

/*
	- 하나의 표면이라도 자동차로 보면 차문, 몸체, 타이어와 같이 서로 다른 재질을 가지고 있다.
	- 서로 다른 재질을 처리하기 위해서 각각의 재질에 따라 값을 설정할 필요가 있다.
	- 각각의 재질을 다르게 하기 위해서 각 점마다 재질 값을 다르게 설정하면 되지만 너무 많은 노력이 있어야 한다.
	- 이를 해결하기 위해 재질마다 상수 버퍼를 이용하여 재질을 서로 다르게 설정하면 된다.
	- DirectX 11에서는 LightHelper.h에 있는 Material를 이용하여 재질의 정보를 담을 수 있다.
*/

#include <DirectXMath.h>

namespace Sameple
{
	struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }

		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;		// w = 반영광 지수 p
		DirectX::XMFLOAT4 Reflect;		// 반사를 나타내는 멤버 변수
	};
}