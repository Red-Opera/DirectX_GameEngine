#include "stdafx.h"

// ========================================================
//	상수 버퍼
// ========================================================

/*
	- HLSL 코드에 cbuffer는 상수 버퍼(Constant Buffer)이다.
	- 상수 버퍼는 의미소로 받은 매개변수와 달리 한번 입력된 상수 버퍼는 HLSL 코드내에서 값을 변경할 수 없다.
	- HLSL에서 4 x 4 행렬을 저장하기 위해서 float4x4라는 형식을 지원하며 이외에 floatNxR(N, R는 1보다 큰 정수)를 지원한다.
	- 상수 버퍼는 C++에서 작성한 코드와 HLSL과 소통을 하는 수단이 된다.
	- 여러 물체마다 세계 행렬이 다르기 때문에 WolrdViewProjection 행렬을 물체마다 갱신해야 한다.
	- 만약 물체가 5개가 있다면 WorldViewProjection 상수 버퍼의 갱신되는 횟수는 프레임당 5번이 된다.
	- 상수 버퍼 갱신을 효율적으로 하기 위해서는 상수 버퍼를 나누어야 한다.

	갱신 빈도에 따른 상수 버퍼를 나누는 예시
		// 물체마다 갱신해야하기 때문에 갱신 빈도가 높은 상수 버퍼
		cbuffer ObjectWorldViewProjection
		{
			float4x4 worldViewProjection;
		};

		// 광원이 움직일때 갱신하기 때문에 갱신 빈도가 보통인 상수 버퍼
		cbuffer LightBuffer
		{
			float3 lightDirection;
			float3 lightPosition;
			float3 lightColor;
		};

		// 갱신 빈도가 거의 없는 상수 버퍼
		cbuffer FogBuffer
		{
			float4 fogColor;
			float fogStart;
			float fogEnd;
		};
*/