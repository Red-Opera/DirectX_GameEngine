#pragma once

class EngineLoop
{
public:
	EngineLoop() = default;
	
	virtual void Initialize() = 0;
	virtual void BeforeFrame() = 0;
	virtual void Start() = 0;
	virtual void LateStart() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Finalize() = 0;
	virtual void Destroy() = 0;

	virtual void OnEnable() = 0;
	virtual void OnDisable() = 0;

	virtual void Reset() = 0;

	virtual ~EngineLoop() = default;
};