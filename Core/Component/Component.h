#pragma once

#include "Core/Object/EngineLoop.h"

#include <string>
#include <memory>

class Component : public EngineLoop
{
#undef GetObject
#undef GetClassName	

public:
	Component() = default;
	virtual ~Component() = default;

	void Initialize() override {}
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

	void SetObject(std::shared_ptr<class Object> object) { this->object = object; }
	std::shared_ptr<class Object> GetObject() { return object; }

	void SetEnable(bool isActive) { this->isActive = isActive; }
	bool GetEnable() const { return isActive; }

	virtual std::string GetClassName() const { return "Component"; }	// ��ü�� ������Ʈ �̸��� ��ȯ�ϴ� �Լ�
	static std::string GetStaticClassName() { return "Component"; }		// �ش� Ŭ������ �̸��� ��ȯ�ϴ� �Լ�

protected:
	std::shared_ptr<class Object> object;

	bool isActive = true;
};