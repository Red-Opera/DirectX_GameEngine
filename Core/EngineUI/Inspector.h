#pragma once

#include "Core/Component/Component.h"
#include "Core/Object/EngineLoop.h"
#include "Core/Object/Object.h"

#include <memory>

class Object;

namespace Engine
{
	class Inspector : public EngineLoop
	{
	public:
		static std::shared_ptr<Inspector> instance;

		static std::shared_ptr<Inspector> GetInstance()
		{
			if (!instance)
				instance = std::make_shared<Inspector>();

			return instance;
		}

		static void SetSelectObject(std::shared_ptr<Object> object) noexcept;
		static std::shared_ptr<Object> GetSelectObject() noexcept;

		Inspector() = default;

		void ShowNodeInfomation() noexcept;

		void ColorObjectEditor() noexcept;
		void ModelEditor() noexcept;
		void MeshComponentEditor() noexcept;
		void CameraEditor() noexcept;

		~Inspector() = default;

		// EngineLoop을(를) 통해 상속됨
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

	private:
		static std::shared_ptr<Object> selectObject;		// 현재 선택된 오브젝트
		static std::shared_ptr<Component> selectComponent;	// 현재 선택된 컴포넌트
	};
}