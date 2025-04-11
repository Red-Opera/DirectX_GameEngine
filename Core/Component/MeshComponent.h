#pragma once

#include "Core/Component/Component.h"
#include "Core/Draw/Mesh.h"

#include <DirectXMath.h>
#include <memory>
#include <string>

#undef GetClassName

struct aiMesh;
class Material;

class MeshComponent : public Component
{
public:
    MeshComponent(std::shared_ptr<class Object> object, std::vector<Mesh*> meshs);
    ~MeshComponent() override = default;

    void Initialize() override;
    void Update() override;

    void CreateMesh(const Material& material, const aiMesh& mesh, float scale = 1.0f);
    void AddMesh(Mesh* meshPtr);

    std::vector<Mesh*>& GetMeshes();

    // Component 클래스 이름 반환 메서드 오버라이드
    virtual std::string GetClassName() const override { return "MeshComponent"; }
    static std::string GetStaticClassName() { return "MeshComponent"; }

private:
    std::vector<Mesh*> meshs;
};
