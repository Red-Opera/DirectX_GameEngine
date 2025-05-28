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

	static void SetActiveCamera(std::shared_ptr<Camera> camera);
	static bool IsActiveCameraVaild();

	static void ResetCamera();

	void SpawnInspectorWidgets();
	void SetRenderPipeline();

	void AddCamera(std::shared_ptr<Camera> camera);
	std::vector<std::shared_ptr<Camera>>& GetCameras();
	Camera* operator->();

	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);
	void Submit(size_t channel) const;

	~CameraContainer();

private:
	static std::shared_ptr<Camera> activeCamera;

	std::vector<std::shared_ptr<Camera>> cameras;

	int active = 0;
};