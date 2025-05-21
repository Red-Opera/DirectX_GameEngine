#pragma once

#include "EngineLoop.h"

#include "Core/Component/Component.h"
#include "Core/Component/TransformComponent.h"
#include "Core/Component/PhysicsComponent.h"

#include <unordered_map>
#include <memory>
#include <concepts>

template<typename T>
concept ComponentChild = std::is_base_of_v<Component, T>;

class Object : public EngineLoop, public std::enable_shared_from_this<Object>
{
public:
	Object(std::string name) : name(name)
    {  
        // 기본적으로 물리 컴포넌트는 비활성화 상태로 시작
        hasPhysics = false;
    }

	static std::shared_ptr<Object> Create(std::string name)
	{
		std::shared_ptr<Object> newObject = std::make_shared<Object>(name);
		
		auto transformComponent = std::make_shared<TransformComponent>(newObject);
		newObject->components[transformComponent->GetClassName()] = transformComponent;
		transformComponent->SetObject(newObject);

		newObject->transform = transformComponent;

		// TransformComponent 자체의 transform 포인터도 업데이트
		transformComponent->transform = transformComponent;

		return newObject;
	}

	void SetName(std::string name) { this->name = name; }
	std::string GetName() const { return name; }

	// 해당 컴포넌트를 추가하는 함수
	template <ComponentChild ComponentClass>
	void AddComponent(std::shared_ptr<ComponentClass> component)
	{
		if (components.find(component->GetClassName()) != components.end())
			return;

		// Component에 Object를 설정함
		component->SetObject(shared_from_this());

		// transform 포인터 동기화 추가
		component->transform = this->transform;

		components[component->GetClassName()] = component;
	}

	template <ComponentChild ComponentClass, typename... Args>
	std::shared_ptr<ComponentClass> AddComponent(Args&&... args)
	{
		// TransformComponent를 추가하려는 경우 기존 transform 반환
		if constexpr (std::is_same_v<ComponentClass, TransformComponent>)
			return std::dynamic_pointer_cast<ComponentClass>(transform);

		// 먼저 이미 해당 컴포넌트가 있는지 확인
		std::string componentName = ComponentClass::GetStaticClassName();

		if (components.find(componentName) != components.end())
			return std::dynamic_pointer_cast<ComponentClass>(components[componentName]);

		// shared_from_this() + 추가 매개변수들을 사용하여 컴포넌트 생성
		auto component = std::make_shared<ComponentClass>(shared_from_this(), std::forward<Args>(args)...);

		// Component에 Object를 설정함
		component->SetObject(shared_from_this());

		// transform 포인터 동기화 추가
		component->transform = this->transform;

		components[component->GetClassName()] = component;

		return component;
	}

	// 템플릿을 사용하여 해당 컴포넌트를 반환하는 함수
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

	// 템플릿을 사용하여 해당 모든 컴포넌트를 반환하는 함수
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

	void SetTransformComponent(std::shared_ptr<TransformComponent> transform);

	// 모든 컴포넌트를 반환하는 함수
	const std::vector<std::shared_ptr<Component>> GetAllComponents();

	void RemoveComponent(std::string componentName);

	// 해당 컴포넌트가 존재하는지 확인하는 함수
	bool HasComponent(std::string componentName) const;

	// 오브젝트 활성화, 비활성화 함수
	void SetActive(bool isActive) { this->isActive = isActive; }
	bool GetActive() const { return isActive; }

    // 물리 속성 활성화 메서드 추가
    void EnablePhysics(bool useGravity = true)
    {
        hasPhysics = true;
        this->useGravity = useGravity;
    }

    bool HasPhysics() const { return hasPhysics; }
    bool UsesGravity() const { return useGravity; }

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

	std::shared_ptr<class TransformComponent> transform;

protected:
	std::unordered_map<std::string, std::shared_ptr<Component>> components;

	std::string name;
	bool isActive = true;
    bool hasPhysics = false;  // 물리 시뮬레이션 적용 여부
    bool useGravity = true;   // 중력 적용 여부
};