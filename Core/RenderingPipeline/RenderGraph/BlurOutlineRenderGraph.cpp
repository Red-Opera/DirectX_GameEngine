#include "stdafx.h"
#include "BlurOutlineRenderGraph.h"

#include "Core/RenderingPipeline/Pipeline/VSPS/DynamicConstantBuffer.h"
#include "Core/RenderingPipeline/RenderingManager/Buffer/BufferPassClear.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/CameraWireFramePass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/HorizontalBlurPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/LambertianRenderPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineDrawPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/OutlineMaskPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/ShadowMapPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/SkyboxPass.h"
#include "Core/RenderingPipeline/RenderingManager/Pass/VerticalBlurPass.h"
#include "Core/RenderingPipeline/RenderGraph/BlurOutlineRenderingPass.h"
#include "Core/RenderingPipeline/RenderTarget.h"

#include "Utility/MathInfo.h"
#include "Utility/Imgui/imgui.h"

namespace RenderGraphNameSpace
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(DxGraphic& graphic)
		: RenderGraph(graphic)
	{
		{
			auto pass = std::make_unique<BufferPassClear>("clearRenderTarget");
			pass->SetSinkLinkage("buffer", "$.backbuffer");

			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<BufferPassClear>("clearDepthStencil");
			pass->SetSinkLinkage("buffer", "$.masterDepth");

			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<ShadowMapPass>(graphic, "ShadowMap");
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<LambertianRenderPass>(graphic, "lambertian");
			pass->SetSinkLinkage("ShadowMap", "ShadowMap.Map");
			pass->SetSinkLinkage("renderTarget", "clearRenderTarget.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDepthStencil.buffer");

			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<SkyboxPass>(graphic, "Skybox");
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");

			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<OutlineMaskPass>(graphic, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "Skybox.depthStencil");

			AddRenderPass(std::move(pass));
		}

		{
			{
				DynamicConstantBuffer::EditLayout layout;
				layout.add<DynamicConstantBuffer::int32>("length");
				layout.add<DynamicConstantBuffer::Array>("sampleCoefficients");
				layout["sampleCoefficients"].set<DynamicConstantBuffer::float1>(maxRadius * 2 + 1);

				DynamicConstantBuffer::Buffer buffer{ std::move(layout) };
				blurKernel = std::make_shared<Graphic::CachingPixelConstantBufferEx>(graphic, buffer, 0);

				SetKernelGauss(radius, sigma);
				AddGlobalProvider(DirectRenderPipelineDataProvider<Graphic::CachingPixelConstantBufferEx>::Create("blurKernel", blurKernel));
			}

			{
				DynamicConstantBuffer::EditLayout layout;
				layout.add<DynamicConstantBuffer::Bool>("isHorizontal");

				DynamicConstantBuffer::Buffer buffer{ std::move(layout) };
				blurDirection = std::make_shared<Graphic::CachingPixelConstantBufferEx>(graphic, buffer, 1);

				AddGlobalProvider(DirectRenderPipelineDataProvider<Graphic::CachingPixelConstantBufferEx>::Create("blurDirection", blurDirection));
			}
		}

		{
			auto pass = std::make_unique<BlurOutlineRenderingPass>(graphic, "outlineDraw", graphic.GetWidth(), graphic.GetHeight());
			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", graphic, graphic.GetWidth(), graphic.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");

			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", graphic);
			pass->SetSinkLinkage("renderTarget", "Skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");

			AddRenderPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<CameraWireFramePass>(graphic, "wireframe");
			pass->SetSinkLinkage("renderTarget", "vertical.renderTarget");
			pass->SetSinkLinkage("depthStencil", "vertical.depthStencil");

			AddRenderPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "wireframe.renderTarget");

		Finalize();
	}

	void BlurOutlineRenderGraph::RenderWindows(DxGraphic& graphic)
	{
		RenderShadowWindow(graphic);
		RenderKernelWindow(graphic);
		dynamic_cast<SkyboxPass&>(FindRenderPass("Skybox")).RenderWidnow();
	}

	void BlurOutlineRenderGraph::RenderKernelWindow(DxGraphic& graphic)
	{
		if (ImGui::Begin("Kernel"))
		{
			bool filterChanged = false;
			{
				const char* items[] = { "Gauss","Box" };
				static const char* curItem = items[0];

				if (ImGui::BeginCombo("Filter Type", curItem))
				{
					for (int n = 0; n < std::size(items); n++)
					{
						const bool isSelected = (curItem == items[n]);

						if (ImGui::Selectable(items[n], isSelected))
						{
							filterChanged = true;
							curItem = items[n];

							if (curItem == items[0])
								kernelType = KernelType::Gauss;

							else if (curItem == items[1])
								kernelType = KernelType::Box;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}

			bool radChange = ImGui::SliderInt("Radius", &radius, 0, maxRadius);
			bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);

			if (radChange || sigChange || filterChanged)
			{
				if (kernelType == KernelType::Gauss)
					SetKernelGauss(radius, sigma);

				else if (kernelType == KernelType::Box)
					SetKernelBox(radius);
			}
		}

		ImGui::End();
	}

	void BlurOutlineRenderGraph::RenderShadowWindow(DxGraphic& graphic)
	{
		if (ImGui::Begin("Shadow"))
		{
			if (ImGui::Button("Dump Cube Map"))
				DumpShadowMap(graphic, "Temp/Dump/");
		}

		ImGui::End();
	}

	void BlurOutlineRenderGraph::RenderMainCamera(Camera& camera)
	{
		dynamic_cast<LambertianRenderPass&>(FindRenderPass("lambertian")).RenderMainCamera(camera);
		dynamic_cast<SkyboxPass&>(FindRenderPass("Skybox")).RenderMainCamera(camera);
	}

	void BlurOutlineRenderGraph::RenderShadowCamera(Camera& camera)
	{
		dynamic_cast<ShadowMapPass&>(FindRenderPass("ShadowMap")).RenderShadowCamera(camera);
		dynamic_cast<LambertianRenderPass&>(FindRenderPass("lambertian")).RenderShadowCamera(camera);
	}

	void BlurOutlineRenderGraph::DumpShadowMap(DxGraphic& grpahic, const std::string& path)
	{
		dynamic_cast<ShadowMapPass&>(FindRenderPass("ShadowMap")).DumpShadowMap(grpahic, path);
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) NOEXCEPTRELEASE
	{
		assert(radius <= maxRadius);

		auto kernel = blurKernel->GetBuffer();
		const int length = radius * 2 + 1;
		kernel["length"] = length;

		float sum = 0.0f;

		for (int i = 0; i < length; i++)
		{
			const auto x = float(i - radius);
			const auto gaussian = Math::Gaussian(x, sigma);

			sum += gaussian;
			kernel["sampleCoefficients"][i] = gaussian;
		}

		for (int i = 0; i < length; i++)
			kernel["sampleCoefficients"][i] = (float)kernel["sampleCoefficients"][i] / sum;

		blurKernel->SetBuffer(kernel);
	}

	void BlurOutlineRenderGraph::SetKernelBox(int radius) NOEXCEPTRELEASE
	{
		assert(radius <= maxRadius);

		auto kernel = blurKernel->GetBuffer();
		const int length = radius * 2 + 1;
		kernel["length"] = length;

		const float c = 1.0f / length;

		for (int i = 0; i < length; i++)
			kernel["sampleCoefficients"][i] = c;

		blurKernel->SetBuffer(kernel);
	}
}