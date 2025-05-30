#pragma once

#include "Core/Object/EngineLoop.h"

#include <string>
#include <memory>

class Component : public EngineLoop
{
#undef GetObject
#undef GetClassName	

public:
	Component(std::shared_ptr<class Object> object);
	virtual ~Component() = default;

	void Initialize() override { }
	void BeforeFrame() override {}
	void Start() override {}
	void LateStart() override {}
	void Update() override {}
	void LateUpdate() override {}
	void Finalize() override {}
	void Destroy() override {}

	void OnEnable() override {}
	void OnDisable() override {}

	void Reset() override {}

	void SetObject(std::shared_ptr<class Object> object);
	std::shared_ptr<class Object> GetObject();

	void SetEnable(bool isEnable);
	bool GetEnable() const { return isActive; }

	virtual std::string GetClassName() const { return "Component"; }	// 객체의 컴포넌트 이름을 반환하는 함수
	static std::string GetStaticClassName() { return "Component"; }		// 해당 클래스의 이름을 반환하는 함수

	std::shared_ptr<class TransformComponent> transform;

protected:
	std::shared_ptr<class Object> object;

	bool isActive = true;
};