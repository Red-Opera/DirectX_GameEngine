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

        // (latDiv - 1) * longDiv + 2 개의 정점
        const unsigned int vertexCount = (latDiv - 1) * longDiv + 2;
        VertexBuffer vertices(std::move(vertexLayout), vertexCount);

        unsigned int idx = 0;

        // --- 위도(극 제외) 정점 배치 ---
        for (int iLat = 1; iLat < latDiv; iLat++)
        {
            // X축 회전
            const auto latBase = dx::XMVector3Transform(base, dx::XMMatrixRotationX(latitudeAngle * iLat));
            for (int iLong = 0; iLong < longDiv; iLong++)
            {
                // Z축 회전
                auto v = dx::XMVector3Transform(latBase, dx::XMMatrixRotationZ(longitudeAngle * iLong));

                // Position
                dx::XMFLOAT3 pos;
                dx::XMStoreFloat3(&pos, v);
                vertices[idx].GetValue<VertexType::Position3D>() = pos;

                idx++;
            }
        }

        // --- 북극 ---
        const unsigned short iNorthPole = idx;
        {
            dx::XMFLOAT3 northPos;
            dx::XMStoreFloat3(&northPos, base);
            vertices[idx].GetValue<VertexType::Position3D>() = northPos;

            idx++;
        }

        // --- 남극 ---
        const unsigned short iSouthPole = idx;
        {
            dx::XMFLOAT3 southPos;
            dx::XMStoreFloat3(&southPos, dx::XMVectorNegate(base));
            vertices[idx].GetValue<VertexType::Position3D>() = southPos;

            idx++;
        }

        // --- 인덱스 계산 ---
        const auto calcIdx = [longDiv](unsigned short iLat, unsigned short iLong) -> unsigned short {
            return iLat * longDiv + iLong;
            };

        std::vector<unsigned short> indices;

        // 본체(밴드) 삼각형
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

            // 경도 wrap
            indices.push_back(calcIdx(iLat, longDiv - 1));
            indices.push_back(calcIdx(iLat + 1, longDiv - 1));
            indices.push_back(calcIdx(iLat, 0));

            indices.push_back(calcIdx(iLat, 0));
            indices.push_back(calcIdx(iLat + 1, longDiv - 1));
            indices.push_back(calcIdx(iLat + 1, 0));
        }

        // 북극 캡
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

        // 남극 캡
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

        // 텍스처 및 노멀 좌표 계산 (구의 극이 Z축에 맞춰져 있음, Sphere는 SetNormalVector()를 사용하지 않음)
        int vertexCount = textureVertices.vertices.count();
        for (int i = 0; i < vertexCount; i++)
        {
            auto& pos = textureVertices.vertices[i].GetValue<VertexType::Position3D>();
            auto& tex = textureVertices.vertices[i].GetValue<VertexType::Texture2D>();
            auto& normal = textureVertices.vertices[i].GetValue<VertexType::Normal>();

            // 벡터 길이 계산
            const float len = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
			const float theta = atan2(pos.y, pos.x);                // 방위각: xy평면에서의 각도 (atan2는 [-π, π] 범위)
            const float phi = acos(pos.z / len);                    // 세로각: 북극(+)인 Z축을 기준으로 한 각도

			const float u = (theta + Math::PI) / (2.0f * Math::PI); // u: [0, 1] 범위
            const float v = phi / Math::PI;                         // v: [0, 1] 범위

            tex.x = u;
            tex.y = v;

			// 노멀 벡터 계산
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