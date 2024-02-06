#pragma once

#include <vector>
#include <DirectXMath.h>
using namespace std;

template<class T>
class TriangleIndexList
{
public:
	TriangleIndexList() = default;
	TriangleIndexList(vector<T> vertices, vector<unsigned short> indices) : vertices(vertices), indices(indices)
	{
		assert("�ﰢ���� ���� ������ �����մϴ�." && vertices.size() > 2);
		assert("�ﰢ���� �ε����� ���� �ʽ��ϴ�." && (indices.size() % 3 == 0));
	}

	void Transform(DirectX::XMMATRIX matrix)
	{
		for (auto& v : vertices)
		{
			DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.position, DirectX::XMVector3Transform(pos, matrix));
		}
	}

	vector<T> vertices;
	vector<unsigned short> indices;
};

