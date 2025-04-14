#pragma once

#include <vector>
#include <memory>

namespace RenderGraphNameSpace { class RenderGraph; }

class Camera;
class DxGraphic;

#undef CreateWindow
class CameraContainer
{
public:
	static Camera& GetActiveCamera();
	static Camera& GetControlCamera();

	static void SetActiveCamera(std::shared_ptr<Camera> camera);
	static void SetControlCamera(std::shared_ptr<Camera> camera);

	static bool IsActiveCameraVaild();
	static bool IsControlCameraVaild();

	static void ResetCamera();

	void CreateWindow();
	void SetRenderPipeline();

	void AddCamera(std::shared_ptr<Camera> camera);
	std::vector<std::shared_ptr<Camera>>& GetCameras();
	Camera* operator->();

	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);
	void Submit(size_t channel) const;

	~CameraContainer();

private:
	static std::shared_ptr<Camera> activeCamera;
	static std::shared_ptr<Camera> controlCamera;

	Camera& GetControlTargetCamera();

	std::vector<std::shared_ptr<Camera>> cameras;

	int active = 0;
	int controlTarget = 0;
};