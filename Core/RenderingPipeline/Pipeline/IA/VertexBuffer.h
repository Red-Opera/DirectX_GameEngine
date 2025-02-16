#pragma once
#include "Core/RenderingPipeline/Render.h"

#include "Core/Exception/GraphicsException.h"
#include "Core/RenderingPipeline/Vertex.h"

namespace Graphic
{
    class VertexBuffer : public Render
    {
    public:
        VertexBuffer(DxGraphic& graphic, const VertexCore::VertexBuffer& vertexBuffer);
        VertexBuffer(DxGraphic& graphic, const std::string& path, const VertexCore::VertexBuffer& vertexBuffer);

        static std::shared_ptr<VertexBuffer> GetRender(DxGraphic& graphic, const std::string& path, const VertexCore::VertexBuffer& vertexBuffer);

        const VertexCore::VertexLayout& GetVertexLayout() const noexcept;

        // Bindable��(��) ���� ��ӵ�
        void SetRenderPipeline(DxGraphic& graphic) NOEXCEPTRELEASE override;

        template<typename...Ignore>
        static std::string CreateID(const std::string& path, Ignore&&...ignore) { return CreateID_(path); }
        std::string GetID() const noexcept override;

    protected:
        std::string path;
        UINT stride;
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        VertexCore::VertexLayout vertexLayout;

    private:
        static std::string CreateID_(const std::string& path);
    };
}