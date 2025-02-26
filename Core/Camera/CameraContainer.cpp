#include "stdafx.h"
#include "CameraContainer.h"
#include "Camera.h"

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"
#include "Utility/Imgui/imgui.h"

void CameraContainer::CreateWindow(DxGraphic& graphic)
{
	if (ImGui::Begin("Cameras"))
	{
		if (ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str()))
		{
			for (int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == active;

				if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
					active = i;
			}

			ImGui::EndCombo();
		}

		if (ImGui::BeginCombo("Control Camera", GetControlTargetCamera().GetName().c_str()))
		{
			for (int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == controlTarget;

				if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
					controlTarget = i;
			}

			ImGui::EndCombo();
		}

		GetControlTargetCamera().SpawnControlWidgets(graphic);
	}

	ImGui::End();
}

void CameraContainer::SetRenderPipeline(DxGraphic& graphic)
{
	graphic.SetCamera((*this)->GetMatrix());
}

void CameraContainer::AddCamera(std::shared_ptr<Camera> camera)
{
	cameras.push_back(std::move(camera));
}

Camera* CameraContainer::operator->()
{
	return &GetActiveCamera();
}

void CameraContainer::LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph)
{
	for (auto& camera : cameras)
		camera->LinkTechniques(renderGraph);
}

void CameraContainer::Submit(size_t channel) const
{
	for (size_t i = 0; i < cameras.size(); i++)
	{
		if (i != active)
			cameras[i]->Submit(channel);
	}
}

Camera& CameraContainer::GetActiveCamera()
{
	return *cameras[active];
}

CameraContainer::~CameraContainer()
{

}

Camera& CameraContainer::GetControlTargetCamera()
{
	return *cameras[controlTarget];
}
