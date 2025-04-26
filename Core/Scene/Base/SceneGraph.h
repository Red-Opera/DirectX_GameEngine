#pragma once

#include "Core/Object/EngineLoop.h"

#include <string>
#include <unordered_map>
#include <functional>

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

    // 객체 선택 공통 로직
    void SelectObject(std::shared_ptr<Object> object);

    // 트리 구조에서 다음/이전 오브젝트 선택 함수
    void SelectNextObjectInTree();
    void SelectPreviousObjectInTree();

    void CollectVisibleObjects(std::vector<std::shared_ptr<Object>>& visibleObjects);                           // 현재 보이는 오브젝트만 수집 (캐시 기반 최적화)
    void CollectChildObjects(std::shared_ptr<Object> parent, std::vector<std::shared_ptr<Object>>& objects);    // 자식 객체 수집 (필요한 경우만 재귀)

    // 부모 캐시 초기화
    void InitializeParentCache();
    void ProcessChildrenForCache(const std::shared_ptr<Object>& parentObj);

    // 부모 객체를 찾는 메소드
    std::shared_ptr<Object> FindParentObject(const std::shared_ptr<Object>& childObject);
    std::shared_ptr<Object> FindParentObjectDirect(const std::shared_ptr<Object>& childObject);
    std::shared_ptr<Object> FindParentInChildrenDirect(const std::shared_ptr<Object>& potentialParent, const std::shared_ptr<Object>& childObject);
    
    // 노드 열림 상태 확인 (인라인 함수로 최적화)
    bool IsNodeOpen(const std::shared_ptr<Object>& object) const;

    // 키 처리 함수
    void HandleRightKeyPress();
    void HandleLeftKeyPress();

    // 참조로 저장해 메모리 할당 회피
    std::vector<std::shared_ptr<class Object>>& sceneObjects;
    std::string sceneName;

    std::shared_ptr<class Object> selectedObject;

    std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<Object>> parentCache;   // 캐싱을 위한 맵 (부모 객체 검색 성능 향상)
    std::unordered_map<std::shared_ptr<Object>, bool> nodeOpenState;                    // 노드 펼침 상태 추적

    // 키 상태 추적 변수
    bool keyDownPressed = false;
    bool keyUpPressed = false;
    bool keyLeftPressed = false;
    bool keyRightPressed = false;
};
