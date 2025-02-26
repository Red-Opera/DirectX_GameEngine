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
	void CreateWindow(DxGraphic& graphic);
	void SetRenderPipeline(DxGraphic& graphic);

	void AddCamera(std::shared_ptr<Camera> camera);
	Camera* operator->();

	void LinkTechniques(RenderGraphNameSpace::RenderGraph& renderGraph);
	void Submit(size_t channel) const;

	Camera& GetActiveCamera();

	~CameraContainer();

private:
	Camera& GetControlTargetCamera();

	std::vector<std::shared_ptr<Camera>> cameras;

	int active = 0;
	int controlTarget = 0;
};