#pragma once
#include <limits>

namespace DynamicConstantBuffer { class Buffer; }

class Technique;
class RenderStep;

class TechniqueBase
{
public:
	void SetTechnique(Technique* tech)
	{
		technique = tech;
		techIndex++;

		OnSetTechnique();
	}

	void SetRenderStep(RenderStep* step)
	{
		renderStep = step;
		bufferIndex++;

		OnSetRenderStep();
	}

	bool VisitBuffer(DynamicConstantBuffer::Buffer& buffer)
	{
		bufferIndex++;

		return OnVisitBuffer(buffer);
	}

	virtual ~TechniqueBase() { }

protected:
	virtual void OnSetTechnique() { }
	virtual void OnSetRenderStep() { }
	virtual bool OnVisitBuffer(DynamicConstantBuffer::Buffer&) { return false; }

	Technique* technique = nullptr;
	RenderStep* renderStep = nullptr;

#undef max
	size_t techIndex = std::numeric_limits<size_t>::max();
	size_t renderStepIndex = std::numeric_limits<size_t>::max();
	size_t bufferIndex = std::numeric_limits<size_t>::max();
};