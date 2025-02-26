#pragma once

#include "Base/RenderQueuePass.h"
#include "Base/RenderJob.h"

#include "Core/Camera/Camera.h"
#include "Core/Draw/BaseModel/Cube.h"
#include "Core/Draw/BaseModel/Sphere.h"
#include "Core/Draw/TextureCube.h"
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
		SkyboxPass(DxGraphic& graphic, std::string name)
			: RenderingPass(std::move(name))
		{
			using namespace Graphic;

			AddDataConsumer(DirectBufferDataConsumer<RenderTarget>::Create("renderTarget", renderTarget));
			AddDataConsumer(DirectBufferDataConsumer<DepthStencil>::Create("depthStencil", depthStencil));

			AddRender(std::make_shared<TextureCube>(graphic, "Images/SpaceBox"));
			AddRender(Stencil::GetRender(graphic, Stencil::DrawMode::DepthFirst));
			AddRender(Rasterizer::GetRender(graphic, true));
			AddRender(std::make_shared<SkyboxTransformConstantBuffer>(graphic));
			AddRender(PixelShader::GetRender(graphic, "Shader/Skybox.hlsl"));
			AddRender(PrimitiveTopology::GetRender(graphic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

			{
				auto vertexShaderCode = VertexShader::GetRender(graphic, "Shader/Skybox.hlsl");

				{
					auto cube = Cube::Create();

					const auto geometryTag = "$Skybox";
					cubeVertexBuffer = VertexBuffer::GetRender(graphic, geometryTag, std::move(cube.vertices));
					cubeIndexBuffer = IndexBuffer::GetRender(graphic, geometryTag, std::move(cube.indices));
					cubeIndexCount = (UINT)cube.indices.size();

					AddRender(InputLayout::GetRender(graphic, cube.vertices.GetVertexLayout(), *vertexShaderCode));
				}

				{
					auto sphere = Sphere::Make();

					const auto geometryTag = "$SkySphere";
					sphereVertexBuffer = VertexBuffer::GetRender(graphic, geometryTag, std::move(sphere.vertices));
					sphereIndexBuffer = IndexBuffer::GetRender(graphic, geometryTag, std::move(sphere.indices));
					sphereIndexCount = (UINT)sphere.indices.size();
				}
				
				AddRender(std::move(vertexShaderCode));
			}

			AddDataProvider(DirectBufferPipelineDataProvider<RenderTarget>::Create("renderTarget", renderTarget));
			AddDataProvider(DirectBufferPipelineDataProvider<DepthStencil>::Create("depthStencil", depthStencil));
		}

		void Execute(DxGraphic& graphic) const NOEXCEPTRELEASE override
		{
			assert(mainCamera != nullptr);
			mainCamera->RenderToGraphic(graphic);

			UINT indexCount;

			if (isSphere)
			{
				sphereVertexBuffer->SetRenderPipeline(graphic);
				sphereIndexBuffer->SetRenderPipeline(graphic);
				indexCount = sphereIndexCount;
			}
			else
			{
				cubeVertexBuffer->SetRenderPipeline(graphic);
				cubeIndexBuffer->SetRenderPipeline(graphic);
				indexCount = cubeIndexCount;
			}

			RenderAll(graphic);
			graphic.DrawIndexed(indexCount);
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