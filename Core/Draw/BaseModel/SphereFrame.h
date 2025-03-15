#pragma once

#include <optional>
#include <DirectXMath.h>

#include "../Base/TriangleIndexList.h"
#include "Core/RenderingPipeline/Vertex.h"

#include "Utility/MathInfo.h"
#include "Utility/Vector.h"

class SphereFrame
{
public:
    static TriangleIndexList MakeTesselated(VertexCore::VertexLayout vertexLayout, int latDiv, int longDiv)
    {
        using namespace VertexCore;
        using VertexType = VertexCore::VertexLayout::VertexType;
        namespace dx = DirectX;

        assert(latDiv >= 3);
        assert(longDiv >= 3);

        constexpr float radius = 1.0f;
        const auto base = dx::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
        const float latitudeAngle = Math::PI / latDiv;
        const float longitudeAngle = 2.0f * Math::PI / longDiv;

        // (latDiv - 1) * longDiv + 2 ���� ����
        const unsigned int vertexCount = (latDiv - 1) * longDiv + 2;
        VertexBuffer vertices(std::move(vertexLayout), vertexCount);

        unsigned int idx = 0;

        // --- ����(�� ����) ���� ��ġ ---
        for (int iLat = 1; iLat < latDiv; iLat++)
        {
            // X�� ȸ��
            const auto latBase = dx::XMVector3Transform(base, dx::XMMatrixRotationX(latitudeAngle * iLat));
            for (int iLong = 0; iLong < longDiv; iLong++)
            {
                // Z�� ȸ��
                auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longitudeAngle * iLong));

                // Position
                dx::XMFLOAT3 pos;
                dx::XMStoreFloat3(&pos, v);
                vertices[idx].GetValue<VertexType::Position3D>() = pos;

                idx++;
            }
        }

        // --- �ϱ� ---
        const unsigned short iNorthPole = idx;
        {
            dx::XMFLOAT3 northPos;
            dx::XMStoreFloat3(&northPos, base);
            vertices[idx].GetValue<VertexType::Position3D>() = northPos;

            idx++;
        }

        // --- ���� ---
        const unsigned short iSouthPole = idx;
        {
            dx::XMFLOAT3 southPos;
            dx::XMStoreFloat3(&southPos, dx::XMVectorNegate(base));
            vertices[idx].GetValue<VertexType::Position3D>() = southPos;

            idx++;
        }

        // --- �ε��� ��� ---
        const auto calcIdx = [longDiv](unsigned short iLat, unsigned short iLong) -> unsigned short {
            return iLat * longDiv + iLong;
            };

        std::vector<unsigned short> indices;

        // ��ü(���) �ﰢ��
        for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
        {
            for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
            {
                indices.push_back(calcIdx(iLat, iLong));
                indices.push_back(calcIdx(iLat + 1, iLong));
                indices.push_back(calcIdx(iLat, iLong + 1));

                indices.push_back(calcIdx(iLat, iLong + 1));
                indices.push_back(calcIdx(iLat + 1, iLong));
                indices.push_back(calcIdx(iLat + 1, iLong + 1));
            }

            // �浵 wrap
            indices.push_back(calcIdx(iLat, longDiv - 1));
            indices.push_back(calcIdx(iLat + 1, longDiv - 1));
            indices.push_back(calcIdx(iLat, 0));

            indices.push_back(calcIdx(iLat, 0));
            indices.push_back(calcIdx(iLat + 1, longDiv - 1));
            indices.push_back(calcIdx(iLat + 1, 0));
        }

        // �ϱ� ĸ
        for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
        {
            indices.push_back(iNorthPole);
            indices.push_back(calcIdx(0, iLong));
            indices.push_back(calcIdx(0, iLong + 1));
        }

        // wrap
        indices.push_back(iNorthPole);
        indices.push_back(calcIdx(0, longDiv - 1));
        indices.push_back(calcIdx(0, 0));

        // ���� ĸ
        for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
        {
            indices.push_back(calcIdx(latDiv - 2, iLong + 1));
            indices.push_back(calcIdx(latDiv - 2, iLong));
            indices.push_back(iSouthPole);
        }
        // wrap
        indices.push_back(calcIdx(latDiv - 2, 0));
        indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
        indices.push_back(iSouthPole);

        return { std::move(vertices), std::move(indices) };
    }

	static TriangleIndexList CreateFrame(std::optional<VertexCore::VertexLayout> vertexLayout = std::nullopt)
	{
		using VertexType = VertexCore::VertexLayout::VertexType;

		if (!vertexLayout)
			vertexLayout = VertexCore::VertexLayout{}.AddType(VertexType::Position3D);

		return MakeTesselated(std::move(*vertexLayout), 12, 24);
	}

    static TriangleIndexList CreateTextureFrame()
    {
        using VertexType = VertexCore::VertexLayout::VertexType;

        auto textureVertices = MakeTesselated(
            VertexCore::VertexLayout{}
            .AddType(VertexType::Position3D)
            .AddType(VertexType::Normal)
            .AddType(VertexType::Texture2D),
            12, 24);

        // �ؽ�ó �� ��� ��ǥ ��� (���� ���� Z�࿡ ������ ����, Sphere�� SetNormalVector()�� ������� ����)
        int vertexCount = textureVertices.vertices.count();
        for (int i = 0; i < vertexCount; i++)
        {
            auto& pos = textureVertices.vertices[i].GetValue<VertexType::Position3D>();
            auto& tex = textureVertices.vertices[i].GetValue<VertexType::Texture2D>();
            auto& normal = textureVertices.vertices[i].GetValue<VertexType::Normal>();

            // ���� ���� ���
            const float len = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
			const float theta = atan2(pos.y, pos.x);                // ������: xy��鿡���� ���� (atan2�� [-��, ��] ����)
            const float phi = acos(pos.z / len);                    // ���ΰ�: �ϱ�(+)�� Z���� �������� �� ����

			const float u = (theta + Math::PI) / (2.0f * Math::PI); // u: [0, 1] ����
            const float v = phi / Math::PI;                         // v: [0, 1] ����

            tex.x = u;
            tex.y = v;

			// ��� ���� ���
            const float x = sin(phi) * cos(theta);
            const float y = sin(phi) * sin(theta);
            const float z = cos(phi);

            normal.x = x;
            normal.y = y;
            normal.z = z;
        }

        return textureVertices;
    }
};