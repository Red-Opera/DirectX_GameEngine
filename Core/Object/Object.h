#pragma once

#include "EngineLoop.h"

#include "Core/Component/Component.h"

#include <unordered_map>
#include <memory>
#include <concepts>

template<typename T>
concept ComponentChild = std::is_base_of_v<Component, T>;

class Object : public EngineLoop, public std::enable_shared_from_this<Object>
{
public:
	Object();

	// 해당 컴포넌트를 추가하는 함수
	template <ComponentChild ComponentClass>
	void AddComponent(std::shared_ptr<ComponentClass> component)
	{
		if (components.find(component->GetName()) != components.end())
			return;

		// Component에 Object를 설정함
		component->SetObject(shared_from_this());

		components[component->GetName()] = *component;
	}

	// 템플릿을 사용하여 해당 컴포넌트를 반환하는 함수
	template <ComponentChild ComponentClass>
	std::shared_ptr<ComponentClass> GetComponent()
	{
		for (auto& component : components)
		{
			if (component.second.GetName() == ComponentClass::GetName())
				return std::dynamic_pointer_cast<ComponentClass>(component.second);
		}

		return nullptr;
	}

	// 템플릿을 사용하여 해당 모든 컴포넌트를 반환하는 함수
	template <ComponentChild ComponentClass>
	const std::vector<std::shared_ptr<ComponentClass>> GetComponents()
	{
		std::vector<std::shared_ptr<ComponentClass>> result;

		for (auto& component : components)
		{
			if (component.second.GetName() == ComponentClass::GetName())
				result.push_back(std::dynamic_pointer_cast<ComponentClass>(component.second));
		}

		return result;
	}

	// 모든 컴포넌트를 반환하는 함수
	const std::vector<std::shared_ptr<Component>> GetAllComponents();

	void RemoveComponent(std::string componentName);

	// 해당 컴포넌트가 존재하는지 확인하는 함수
	bool HasComponent(std::string componentName) const;

	// 오브젝트 활성화, 비활성화 함수
	void SetActive(bool isActive) { this->isActive = isActive; }
	bool GetActive() const { return isActive; }

	void Initialize() override;
	void BeforeFrame() override;
	void Start() override;
	void LateStart() override;
	void Update() override;
	void LateUpdate() override;
	void Finalize() override;
	void Destroy() override;

	void OnEnable() override;
	void OnDisable() override;

	void Reset() override;

	~Object() = default;

private:
	std::unordered_map<std::string, std::shared_ptr<Component>> components;

	bool isActive = true;
};