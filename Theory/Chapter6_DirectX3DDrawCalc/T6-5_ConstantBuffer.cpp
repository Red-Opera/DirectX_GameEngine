#include "stdafx.h"

// ========================================================
//	��� ����
// ========================================================

/*
	- HLSL �ڵ忡 cbuffer�� ��� ����(Constant Buffer)�̴�.
	- ��� ���۴� �ǹ̼ҷ� ���� �Ű������� �޸� �ѹ� �Էµ� ��� ���۴� HLSL �ڵ峻���� ���� ������ �� ����.
	- HLSL���� 4 x 4 ����� �����ϱ� ���ؼ� float4x4��� ������ �����ϸ� �̿ܿ� floatNxR(N, R�� 1���� ū ����)�� �����Ѵ�.
	- ��� ���۴� C++���� �ۼ��� �ڵ�� HLSL�� ������ �ϴ� ������ �ȴ�.
	- ���� ��ü���� ���� ����� �ٸ��� ������ WolrdViewProjection ����� ��ü���� �����ؾ� �Ѵ�.
	- ���� ��ü�� 5���� �ִٸ� WorldViewProjection ��� ������ ���ŵǴ� Ƚ���� �����Ӵ� 5���� �ȴ�.
	- ��� ���� ������ ȿ�������� �ϱ� ���ؼ��� ��� ���۸� ������� �Ѵ�.

	���� �󵵿� ���� ��� ���۸� ������ ����
		// ��ü���� �����ؾ��ϱ� ������ ���� �󵵰� ���� ��� ����
		cbuffer ObjectWorldViewProjection
		{
			float4x4 worldViewProjection;
		};

		// ������ �����϶� �����ϱ� ������ ���� �󵵰� ������ ��� ����
		cbuffer LightBuffer
		{
			float3 lightDirection;
			float3 lightPosition;
			float3 lightColor;
		};

		// ���� �󵵰� ���� ���� ��� ����
		cbuffer FogBuffer
		{
			float4 fogColor;
			float fogStart;
			float fogEnd;
		};
*/