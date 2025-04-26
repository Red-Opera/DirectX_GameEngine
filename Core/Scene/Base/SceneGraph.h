#pragma once

#include "Core/Object/EngineLoop.h"

#include <string>

class SceneGraph : public EngineLoop
{
public:
    SceneGraph(std::string sceneName, std::vector<std::shared_ptr<class Object>>& sceneObjects);

    void ShowNodeChildren(std::shared_ptr<Object> object) noexcept;
    void UpdateSceneGraph() noexcept;

    void EnableOutlineForObject(std::shared_ptr<Object> object) noexcept;
    void DisableOutlineForObject(std::shared_ptr<Object> object) noexcept;

    ~SceneGraph() = default;

    void Initialize() override {}
    void BeforeFrame() override {}
    void Start() override {}
    void LateStart() override {}
    void LateUpdate() override {}
    void Update() override;
    void Finalize() override {}
    void Destroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void Reset() override {}

private:
    void SetOutlineForObject(std::shared_ptr<Object> object, bool enable) noexcept;
    void SetOutlineForSingleObject(std::shared_ptr<Object> object, bool enable) noexcept;
    void SetOutlineForChildren(std::shared_ptr<Object> object, bool enable) noexcept;

    std::vector<std::shared_ptr<class Object>>& sceneObjects;    // 씬에 포함된 오브젝트를 저장하는 배열
    std::string sceneName;

    std::shared_ptr<class Object> selectedObject;    // 선택된 노드의 이름
};
