#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"
	
#include "Core/Camera/Camera.h"
#include "Core/Draw/BaseModel/TextureCube.h"
#include "Core/RenderingPipeline/Pipeline/OM/ColorBlend.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/NullPixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/PixelShader.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/VertexShader.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/ShadowRasterizer.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/Rasterizer.h"
#include "Core/RenderingPipeline/Viewport.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"
#include "Core/RenderingPipeline/RenderTarget.h"

#include "Utility/MathInfo.h"

#include <vector>


class DxGraphic;

namespace RenderGraphNameSpace
{
	class ShadowMapPass : public RenderQueuePass
	{
	public:
		ShadowMapPass(std::string name)
			: RenderQueuePass(std::move(name))
		{
			using namespace Graphic;

			depthTextureCube = std::make_unique<DepthTextureCube>(size, 3);

			AddRender(VertexShader::GetRender("Shader/ShadowVS.hlsl"));
			AddRender(NullPixelShader::GetRender());
			AddRender(Stencil::GetRender(Stencil::DrawMode::Off));
			AddRender(ColorBlend::GetRender(false));
			AddRender(std::make_shared<Viewport>((float)size, (float)size));
			AddRender(std::make_shared<Graphic::ShadowRasterizer>(50, 2.0f, 0.1f));

			AddDataProvider(DirectRenderPipelineDataProvider<Graphic::DepthTextureCube>::Create("Map", depthTextureCube));

			DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveFovLH(Math::PI / 2.0f, 1.0f, 0.5f, 100.0f));

			DirectX::XMStoreFloat3(&cameraDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUp[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUp[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUp[2], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUp[3], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUp[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			DirectX::XMStoreFloat3(&cameraDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUp[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			SetDepthStencil(depthTextureCube->GetDepthStencil(0));
		}

		void Execute() const NOEXCEPTRELEASE override
		{
			XMFLOAT3 cameraPosition = shadowCamera->GetPosition();
			const auto position = XMLoadFloat3(&cameraPosition);

			Window::GetDxGraphic().SetProjection(XMLoadFloat4x4(&projection));

			for (size_t i = 0; i < 6; i++)
			{
				auto depth = depthTextureCube->GetDepthStencil(i);
				depth->Clear();

				SetDepthStencil(std::move(depth));
				
				const auto lookAt = position + XMLoadFloat3(&cameraDirections[i]);
				Window::GetDxGraphic().SetCamera(XMMatrixLookAtLH(position, lookAt, XMLoadFloat3(&cameraUp[i])));

				RenderQueuePass::Execute();
			}
		}

		void DumpShadowMap(const std::string& path) const
		{
			for (size_t i = 0; i < 6; i++)
			{
				auto depth = depthTextureCube->GetDepthStencil(i);
				depth->ToImage().Save(path + std::to_string(i) + ".png");
			}
		}

		void RenderShadowCamera(const Camera& camera) noexcept
		{
			shadowCamera = &camera;
		}

	private:
		void SetDepthStencil(std::shared_ptr<Graphic::DepthStencil> depthStencil) const
		{
			const_cast<ShadowMapPass*>(this)->depthStencil = std::move(depthStencil);
		}

		const Camera* shadowCamera = nullptr;

		std::vector<DirectX::XMFLOAT3> cameraUp{ 6 };
		std::vector<DirectX::XMFLOAT3> cameraDirections{ 6 };

		DirectX::XMFLOAT4X4 projection;

		std::shared_ptr<Graphic::DepthTextureCube> depthTextureCube;

		static constexpr UINT size = 1000;
	};
}