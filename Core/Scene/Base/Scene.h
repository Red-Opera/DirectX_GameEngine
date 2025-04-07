#pragma once

#include "Core/Object/Object.h"
#include "Core/Scene/Base/SceneGraph.h"

#include <memory>
#include <vector>

#undef GetObject

class Scene : public EngineLoop
{
public:
	Scene(std::string sceneName);

	static std::shared_ptr<Scene> Create(std::string sceneName);

	std::shared_ptr<Object> AddObject(std::shared_ptr<Object> object);	// 오브젝트를 추가하는 메소드
	std::shared_ptr<Object> RemoveObject(const std::string& name);		// 해당 이름을 가진 오브젝트를 제거하는 메소드
	std::shared_ptr<Object> RemoveObject(size_t index);					// 해당 인덱스에 있는 오브젝트를 제거하는 메소드

	std::shared_ptr<Object> GetObject(const std::string& name);			// 해당 이름을 가진 오브젝트를 반환하는 메소드
	std::shared_ptr<Object> GetObject(size_t index);					// 해당 인덱스에 있는 오브젝트를 반환하는 메소드
	std::vector<std::shared_ptr<Object>> GetObjects();					// 씬에 포함된 모든 오브젝트를 반환하는 메소드

	std::string GetName() const;										// 씬의 이름을 반환하는 메소드

	// 해당 이름을 가진 오브젝트가 존재하는지 확인하는 메소드
	bool HasObject(const std::string& name);

	// 현재 활성화된 씬을 반환하는 메소드
	static std::shared_ptr<Scene> GetActiveScene();

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

protected:
	// 현재 활성화된 씬을 저장하는 정적 변수
	static std::shared_ptr<Scene> activeScene;

	std::unordered_map<std::string, size_t> objectIndex;	// 오브젝트 이름과 인덱스를 저장하는 해시맵
	std::vector<std::shared_ptr<Object>> objects;			// 씬에 포함된 오브젝트를 저장하는 배열

	std::string sceneName;									// 씬 이름

	std::shared_ptr<SceneGraph> sceneGraph;					// 씬 그래프
};