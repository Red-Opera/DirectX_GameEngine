#include "stdafx.h"
#include "CameraContainer.h"
#include "Camera.h"

#include "Core/DxGraphic.h"
#include "Core/RenderingPipeline/RenderGraph/RenderGraph.h"
#include "Core/Window.h"
#include "Core/Scene/Base/Scene.h"

#include "External/Imgui/imgui.h"

std::shared_ptr<Camera> CameraContainer::activeCamera;
std::shared_ptr<Camera> CameraContainer::controlCamera;

Camera& CameraContainer::GetActiveCamera()
{
	if (activeCamera == nullptr)
		activeCamera = Scene::GetActiveScene()->GetCameraContainer().GetCameras()[0];

	return *activeCamera;
}

Camera& CameraContainer::GetControlCamera()
{
	if (controlCamera == nullptr)
		controlCamera = Scene::GetActiveScene()->GetCameraContainer().GetCameras()[0];

	return *controlCamera;
}

void CameraContainer::SetActiveCamera(std::shared_ptr<Camera> camera)
{
	activeCamera = camera;

	if (controlCamera == nullptr)
		controlCamera = activeCamera;
}

void CameraContainer::SetControlCamera(std::shared_ptr<Camera> camera)
{
	controlCamera = camera;

	if (activeCamera == nullptr)
		activeCamera = controlCamera;
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

bool CameraContainer::IsControlCameraVaild()
{
	if (controlCamera == nullptr)
	{
		auto scene = Scene::GetActiveScene();
		auto& cameraContainer = scene->GetCameraContainer();
		auto& cameras = cameraContainer.GetCameras();

		if (cameras.size() == 0)
			return false;

		SetControlCamera(cameras[0]);
	}

	return controlCamera != nullptr;
}

void CameraContainer::ResetCamera()
{
	activeCamera = nullptr;
	controlCamera = nullptr;
}

void CameraContainer::CreateWindow()
{
	if (ImGui::Begin("Cameras"))
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

		if (ImGui::BeginCombo("Control Camera", GetControlTargetCamera().GetName().c_str()))
		{
			for (int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == controlTarget;

				if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
				{
					controlTarget = i;

					controlCamera = cameras[i];
				}
			}

			ImGui::EndCombo();
		}

		GetControlTargetCamera().SpawnControlWidgets();
	}

	ImGui::End();
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

	if (controlCamera == nullptr)
		controlCamera = cameras[cameras.size() - 1];
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

Camera& CameraContainer::GetControlTargetCamera()
{
	return *cameras[controlTarget];
}
