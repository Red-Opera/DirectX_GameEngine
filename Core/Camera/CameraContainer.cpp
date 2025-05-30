#include "stdafx.h"
#include "CameraContainer.h"
#include "Camera.h"

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"
#include "Core/Window.h"
#include "Core/Scene/Base/Scene.h"

#include "External/Imgui/imgui.h"

std::shared_ptr<Camera> CameraContainer::activeCamera;

Camera& CameraContainer::GetActiveCamera()
{
	if (activeCamera == nullptr)
		activeCamera = Scene::GetActiveScene()->GetCameraContainer().GetCameras()[0];

	return *activeCamera;
}

void CameraContainer::SetActiveCamera(std::shared_ptr<Camera> camera)
{
	activeCamera = camera;
}

bool CameraContainer::IsActiveCameraVaild()
{
	if (activeCamera == nullptr)
	{
		auto scene = Scene::GetActiveScene();
		auto& cameraContainer = scene->GetCameraContainer();
		auto& cameras = cameraContainer.GetCameras();

		if (cameras.size() == 0)
			return false;

		SetActiveCamera(cameras[0]);
	}

	return activeCamera != nullptr;
}

void CameraContainer::ResetCamera()
{
	activeCamera = nullptr;
}

void CameraContainer::SpawnInspectorWidgets()
{
	if (ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str()))
	{
		for (int i = 0; i < std::size(cameras); i++)
		{
			const bool isSelected = i == active;

			if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
			{
				active = i;
				activeCamera = cameras[i];
			}
		}

		ImGui::EndCombo();
	}
}

void CameraContainer::SetRenderPipeline()
{
	Window::GetDxGraphic().SetCamera((*this)->GetMatrix());
}

void CameraContainer::AddCamera(std::shared_ptr<Camera> camera)
{
	if (cameras.size() == 0)
		CameraContainer::ResetCamera();

	cameras.push_back(std::move(camera));

	if (activeCamera == nullptr)
		activeCamera = cameras[cameras.size() - 1];
}

std::vector<std::shared_ptr<Camera>>& CameraContainer::GetCameras()
{
	return cameras;
}

Camera* CameraContainer::operator->()
{
	return cameras[active].get();
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

CameraContainer::~CameraContainer()
{

}