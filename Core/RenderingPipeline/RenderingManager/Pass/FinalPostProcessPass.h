#pragma once

#include "PostProcessFullScreenRenderPass.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/ConstantBufferEx.h"

namespace Graphic { class RenderTarget; }

namespace RenderGraphNameSpace
{
    class FinalPostProcessPass : public PostProcessFullScreenRenderPass
    {
    public:
        FinalPostProcessPass(std::string name, unsigned int width, unsigned int height);
        void Execute() NOEXCEPTRELEASE override;

    private:
        std::shared_ptr<Graphic::CachingPixelConstantBufferEx> direction;
    };
}
