#pragma once

struct Color
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Normal;
	XMFLOAT2 LT;
	XMFLOAT2 RB;
};

class IA
{
public:
	IA();
};