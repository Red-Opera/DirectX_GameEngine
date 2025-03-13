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

	std::string GetName() const { return name; }

	// �ش� ������Ʈ�� �߰��ϴ� �Լ�
	template <ComponentChild ComponentClass>
	void AddComponent(std::shared_ptr<ComponentClass> component)
	{
		if (components.find(component->GetClassName()) != components.end())
			return;

		// Component�� Object�� ������
		component->SetObject(shared_from_this());

		components[component->GetClassName()] = *component;
	}

	// ���ø��� ����Ͽ� �ش� ������Ʈ�� ��ȯ�ϴ� �Լ�
	template <ComponentChild ComponentClass>
	std::shared_ptr<ComponentClass> GetComponent()
	{
		for (auto& component : components)
		{
			if (component.second->GetClassName() == ComponentClass::GetStaticClassName())
				return std::dynamic_pointer_cast<ComponentClass>(component.second);
		}

		return nullptr;
	}

	// ���ø��� ����Ͽ� �ش� ��� ������Ʈ�� ��ȯ�ϴ� �Լ�
	template <ComponentChild ComponentClass>
	const std::vector<std::shared_ptr<ComponentClass>> GetComponents()
	{
		std::vector<std::shared_ptr<ComponentClass>> result;

		for (auto& component : components)
		{
			if (component.second.GetClassName() == ComponentClass::GetClassName())
				result.push_back(std::dynamic_pointer_cast<ComponentClass>(component.second));
		}

		return result;
	}

	const std::shared_ptr<Component> GetComponent(std::string componentName)
	{
		if (components.find(componentName) == components.end())
			return nullptr;

		return components[componentName];
	}

	// ��� ������Ʈ�� ��ȯ�ϴ� �Լ�
	const std::vector<std::shared_ptr<Component>> GetAllComponents();

	void RemoveComponent(std::string componentName);

	// �ش� ������Ʈ�� �����ϴ��� Ȯ���ϴ� �Լ�
	bool HasComponent(std::string componentName) const;

	// ������Ʈ Ȱ��ȭ, ��Ȱ��ȭ �Լ�
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

	std::string name;
	bool isActive = true;
};