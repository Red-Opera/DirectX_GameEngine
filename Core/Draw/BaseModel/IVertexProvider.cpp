#include "stdafx.h"

#include "IVertexProvider.h"
#include "Core/Component/Transform/Transform.h"
#include "Utility/Vector.h"

bool IVertexProvider::ExtractPhysXVertices(const TriangleIndexList& model,
                                         std::vector<physx::PxVec3>& vertices,
                                         std::vector<uint32_t>& indices,
                                         const Scale& scale)
{
    using VertexType = VertexCore::VertexLayout::VertexType;
    
    vertices.clear();
    indices.clear();
    
    try
    {
        // 정점 데이터 추출
        size_t vertexCount = model.vertices.count();
        vertices.reserve(vertexCount);
        
        for (size_t i = 0; i < vertexCount; i++)
        {
            auto pos = model.vertices[i].GetValue<VertexType::Position3D>();
            
            // 스케일 적용
            vertices.emplace_back(
                pos.x * scale.x,
                pos.y * scale.y,
                pos.z * scale.z
            );
        }
        
        // 인덱스 데이터 추출
        indices.reserve(model.indices.size());
        for (auto index : model.indices)
        {
            indices.push_back(static_cast<uint32_t>(index));
        }
        
        return true;
    }

    catch (...)
    {
        return false;
    }
}