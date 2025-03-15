#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "Core/Camera/Camera.h"
#include "Core/Draw/BaseModel/CubeFrame.h"
#include "Core/Draw/BaseModel/SphereFrame.h"
#include "Core/Draw/BaseModel/TextureCube.h"
#include "Core/RenderingPipeline/Pipeline/IA/PrimitiveTopology.h"
#include "Core/RenderingPipeline/Pipeline/IA/IndexBuffer.h"
#include "Core/RenderingPipeline/Pipeline/IA/InputLayout.h"
#include "Core/RenderingPipeline/Pipeline/IA/VertexBuffer.h"
#include "Core/RenderingPipeline/Pipeline/OM/Stencil.h"
#include "Core/RenderingPipeline/Pipeline/OM/DepthStencil.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SamplerState.h"
#include "Core/RenderingPipeline/Pipeline/VSPS/SkyboxTransformConstantBuffer.h"
#include "Core/RenderingPipeline/Pipeline/Rasterizer/Rasterizer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataConsumer.h"
#include "Core/RenderingPipeline/RenderGraph/PipelineDataProvider.h"

#include <vector>

class DxGraphic;

namespace RenderGraphNameSpace
{
	class SkyboxPass : public RenderingPass
	{
	public:
		SkyboxPass(std::string name)
			: RenderingPass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));
			AddDataConsumer(DirectBufferDataConsumer<DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(std::make_shared<TextureCube>("Images/SpaceBox"));
			AddRender(Stencil::GetRender(Stencil::DrawMode::DepthFirst));
			AddRender(Rasterizer::GetRender(true));
			AddRender(std::make_shared<SkyboxTransformConstantBuffer>());
			AddRender(PixelShader::GetRender("Shader/Skybox.hlsl"));
			AddRender(PrimitiveTopology::GetRender(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			{
				auto vertexShaderCode = VertexShader::GetRender("Shader/Skybox.hlsl");

				{
					auto cube = CubeFrame::CreateFrame();

					const auto geometryTag = "$Skybox";
					cubeVertexBuffer = VertexBuffer::GetRender(geometryTag, std::move(cube.vertices));
					cubeIndexBuffer = IndexBuffer::GetRender(geometryTag, std::move(cube.indices));
					cubeIndexCount = (UINT)cube.indices.size();

					AddRender(InputLayout::GetRender(cube.vertices.GetVertexLayout(), *vertexShaderCode));
				}

				{
					auto sphere = SphereFrame::CreateFrame();

					const auto geometryTag = "$SkySphere";
					sphereVertexBuffer = VertexBuffer::GetRender(geometryTag, std::move(sphere.vertices));
					sphereIndexBuffer = IndexBuffer::GetRender(geometryTag, std::move(sphere.indices));
					sphereIndexCount = (UINT)sphere.indices.size();
				}
				
				AddRender(std::move(vertexShaderCode));
			}

			AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
			AddDataProvider(DirectBufferPipelineDataProvider<DepthStencil>::Create("depthStencil", depthStencil));
		}

		void Execute() const NOEXCEPTRELEASE override
		{
			assert(mainCamera != nullptr);
			mainCamera->RenderToGraphic();

			UINT indexCount;

			if (isSphere)
			{
				sphereVertexBuffer->SetRenderPipeline();
				sphereIndexBuffer->SetRenderPipeline();
				indexCount = sphereIndexCount;
			}
			else
			{
				cubeVertexBuffer->SetRenderPipeline();
				cubeIndexBuffer->SetRenderPipeline();
				indexCount = cubeIndexCount;
			}

			RenderAll();
			Window::GetDxGraphic().DrawIndexed(indexCount);
		}

		void RenderWidnow()
		{
			if (ImGui::Begin("Skybox"))
				ImGui::Checkbox("Use Sphere", &isSphere);

			ImGui::End();
		}

		void RenderMainCamera(const Camera& camera) noexcept
		{
			mainCamera = &camera;
		}

	private:
		const Camera* mainCamera = nullptr;

		std::shared_ptr<Graphic::VertexBuffer> cubeVertexBuffer;
		std::shared_ptr<Graphic::IndexBuffer> cubeIndexBuffer;
		UINT cubeIndexCount;

		std::shared_ptr<Graphic::VertexBuffer> sphereVertexBuffer;
		std::shared_ptr<Graphic::IndexBuffer> sphereIndexBuffer;
		UINT sphereIndexCount;

		bool isSphere = true;
	};
}